#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <limits>
#include <vector>
#include <numeric>
#include <algorithm>

//Stopped at 13:01

enum class OrderType
{
    GoodTillCancel,
    FillAndKill 
};

enum class Side
{
    Buy, 
    Sell
}; 

//Alias integers for clearer typing
using Price = std::int32_t;
using Quantity = std::uint32_t; 
using OrderId = std::uint64_t; 

//Levels are made up of price and quantity, used in public API's to get state of the orderbook
struct LevelInfo
{
    Price price_; 
    Quantity quantity_; 
}; 

using LevelInfos = std::vector<LevelInfo>; 

class OrderBookLevelInfos
{
public: 
    OrderBookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
        : bids_{bids}
        , asks_{asks}
    { }

    const LevelInfos& GetBids() const {return bids_;} 
    const LevelInfos& GetAsks() const {return asks_;}

private: 
    LevelInfos bids_;
    LevelInfos asks_; 
};

class Order
{
public: 
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_ { orderType }
    , orderId_ { orderId }
    , side_ { side }
    , price_ { price }
    , initialQuantity_ { quantity }
    , remainingQuantity_ { quantity }
    { }

    OrderType GetOrderType() const { return orderType_; }
    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }      
    Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetFilledQuantity(); }

    bool IsFilled() const { return GetRemainingQuantity() == 0; }

    void Fill(Quantity quantity){
        if (quantity > GetRemainingQuantity())
            throw std::logic_error( std::format("Impossible for Order ({}) to be filled for more than its remining quantity."), GetOrderId() )
    
        remainingQuantity_ -= quantity;
    }

private: 
    OrderType orderType_;
    OrderId orderId_; 
    Side side_; 
    Price price_; 
    Quantity initialQuantity_; 
    Quantity remainingQuantity_; 
};

//Smart pointer: 
//Automatic memory management, Reference counting, Thread safe reference count

//Stored in orders dictionary and can be in bids and asks
using OrderPointer = std::shared_ptr<Order>; //Smart pointer? 
using OrderPointers = std::list<OrderPointer>; //Might want to use a vector for optimization

//Adding support for modifying order to change side
class OrderModify
{
public: 
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
    : orderId_{orderId}
    , price_{price}
    , side_{side}
    , quantity_{quantity}

    OrderId GetOrderId() const {return orderId_;} 
    Price GetPrice() const {return price_;}
    Side GetSide() const {return side_;}
    Quantity GetQuantity() const {return quantity_;}

    //Only two order types are Fill and Kill and GTC. Can only modify GTC, but might want to support other order types. 
    OrderPointer ToOrderPointer(OrderType type) const
    {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

private: 
    OrderId orderId_; 
    Price price_; 
    Side side_; 
    Quantity quantity_; 
}; 

struct TradeInfo
{
    orderId orderId_; 
    Price price_; 
    Quantity quantity_; 
}; 

class Trade
{
public: 
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
        : bidTrade_{bidTrade}
        , askTrade_{askTrade}
    {}

    const TradeInfo& GetBidTrade() const {return bidTrade_;}
    const TradeInfo& GetAskTrade() const {return askTrade_;}

private: 
    TradeInfo bidTrade_; 
    TradeInfo askTrade_; 
}

using Trades = std::vector<Trade>; 

class Orderbook
{
private: 
    
    struct OrderEntry
    {
        OrderPointer order_{nullptr};
        OrderPointers::iterator location_; 
    }

    std::map<Price, OrderPointers, std::greater<Price>> bids_; //Use red-black tree
    std::map<Price, OrderPointers, std::less<Price>> asks_; 
    std::unordered_map<OrderId, OrderEntry> orders_; //Hash table

    //Match method, and can match method
    //When a fill and kill order is sent, if it can't fill anything in orderbook, it is cancelled

    bool CanMatch(Side side, Price price) const
    {
        if (side == Side::Buy)
        {
            if (asks_.empty() )
                return false; 
            
            const auto& [bestAsk, _] = *asks_.begin();

            return price >= bestAsk; 
        }

        else
        {
            if ( bids_.empty() )
                return false; 
            
            const auto& [bestBid, _] = *bids_.begin(); 
            
            return price <= bestBid; 
        }
    }

    Trades MatchOrders()
    {
        Trades trades; 
        trades.reserve( orders_.size() ); 

        while (true)
        {
            if ( bids_.empty() || asks_.empty() )
                break;
            
            auto& [ bidPrice, bids ] = bids_.begin()
            auto& [ askPrice, asks ] = asks_.begin()

            if (bidPrice < askPrice)
                break; 
            
            while ( bids_size() && asks.size() )
            {
                auto& bid = bids.front();
                auto& ask = asks.front();

                Quantity quantity = std::min( bid->GetRemainingQuantity(), ask-> GetRemainingQuantity() ); 

                bid->Fill(quantity); 
                ask->Fill(quantity); 

                if ( bid->IsFilled() )
                {
                    bids.pop_front(); 
                    orders_.erase( bid->GetOrderId() );
                }

                if ( ask->IsFilled() )
                {
                    asks.pop_front(); 
                    orders_.erase( ask->GetOrderId() );
                }
            }
        }
    }
}

int main()
{
    return 0; 
}