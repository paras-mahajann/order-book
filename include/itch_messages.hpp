#pragma once
#include<variant>
#include<cstdint>
#include<array>


enum class MessageType : char {
	AddOrder = 'A',
	OrderExecuted = 'E',
	OrderCancel = 'X',
	OrderDelete = 'D',
	Trade = 'P'
};

struct AddOrderMessage {
  uint64_t orderRefNum;
  char side;  //buy or sell
  uint32_t shares;
  std::array<char, 8> symbol;
  uint32_t price; // in dollars
};

struct OrderExecutedMessage {
  uint64_t orderRefNum;
  uint32_t executedShares;
};

struct OrderCancelMessage {
  uint64_t orderRefNum;
  uint32_t cancelledShares;
};

struct OrderDeleteMessage {
  uint64_t orderRefNum;
};

struct TradeMessage {
  uint64_t orderRefNum;
  uint32_t shares;
  std::array<char, 8> symbol;
  uint32_t price;
};

using ITCHMessage = std::variant
	<
	AddOrderMessage, 
	OrderExecutedMessage,
    OrderCancelMessage, 
	OrderDeleteMessage, 
	TradeMessage 
	> ;