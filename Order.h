#pragma once

#include <list>
#include <format>
#include <exception>

#include "OrderType.h"
#include "Side.h"
#include "Usings.h"

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
    Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }

    bool IsFilled() const { return GetRemainingQuantity() == 0; }

    void Fill(Quantity quantity){
        if (quantity > GetRemainingQuantity())
            throw std::logic_error( 
                std::format("Impossible for Order ({}) to be filled for more than its remining quantity.", GetOrderId()) 
            );
    
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