#pragma once

#include "Usings.h"

//Levels are made up of price and quantity, used in public API's to get state of the orderbook
struct LevelInfo
{
    Price price_; 
    Quantity quantity_; 
}; 

using LevelInfos = std::vector<LevelInfo>; 