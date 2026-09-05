#pragma once
#include "byte_reader.hpp"
#include "itch_messages.hpp"
#include<stdexcept>
#include<cstring>
#include<vector>

inline AddOrderMessage parseAddOrder(const uint8_t* buffer) {
  AddOrderMessage msg;
  msg.orderRefNum = readBigEndian64(buffer + 1);
  msg.side = static_cast<char>(buffer[9]);
  msg.shares = readBigEndian32(buffer + 10);
  msg.symbol = readSymbol(buffer + 14);
  msg.price = readBigEndian32(buffer + 22);
  return msg;
}

inline OrderExecutedMessage parseOrderExecuted(const uint8_t* buffer) {
  OrderExecutedMessage msg;
  msg.orderRefNum = readBigEndian64(buffer + 1);
  msg.executedShares = readBigEndian32(buffer + 9);
  return msg;
}

inline OrderDeleteMessage parseOrderDelete(const uint8_t* buffer) {
  OrderDeleteMessage msg;
  msg.orderRefNum = readBigEndian64(buffer + 1);
  return msg;
}

inline OrderCancelMessage parseOrderCancel(const uint8_t* buffer) {
  OrderCancelMessage msg;
  msg.orderRefNum = readBigEndian64(buffer + 1);
  msg.cancelledShares = readBigEndian32(buffer + 9);
  return msg;
}

inline TradeMessage parseTrade(const uint8_t* buffer) {
    TradeMessage msg;
    msg.orderRefNum = readBigEndian64(buffer + 1);
    msg.shares = readBigEndian32(buffer + 9);
    msg.symbol = readSymbol(buffer+13);
    msg.price = readBigEndian32(buffer + 21);
    return msg;
}

inline ITCHMessage parseMessage(const uint8_t* buffer) {
  char type = static_cast<char>(buffer[0]);
  switch (type) {
  case 'A':
    return parseAddOrder(buffer);
  case 'E':
    return parseOrderExecuted(buffer);
  case 'X':
    return parseOrderCancel(buffer);
  case 'D':
    return parseOrderDelete(buffer);
  case 'P':
    return parseTrade(buffer);
  default:
    throw std::runtime_error("Unknown message type");
  }
}

inline std::vector<uint8_t> buildAddOrderMessage(uint64_t orderRefNum, char side,
    uint32_t shares, const char* symbol,
    uint32_t price) {
  std::vector<uint8_t> buf(26, 0);

  buf[0] = 'A';

  for (int i = 0; i < 8; i++) {
    buf[1 + i] = static_cast<uint8_t>(orderRefNum >> (56 - i * 8));
  }

  buf[9] = static_cast<uint8_t>(side);

  for (int i = 0; i < 4; i++) {
    buf[10 + i] = static_cast<uint8_t>(shares >> (24 - i * 8));
  }

  for (int i = 0;i < 8;i++) {
    buf[14 + i] = (i < static_cast<int>(strlen(symbol))) ? symbol[i] : ' ';
  }

  for (int i = 0; i < 4; i++) {
    buf[22 + i] = static_cast<uint8_t>(price >> (24 - i * 8));
  }

  return buf;
}

inline std::vector<uint8_t> buildOrderDeleteMessage(uint64_t orderRefNum) {
  std::vector<uint8_t> buf(9, 0);
  buf[0] = 'D';
  for (int i = 0; i < 8; i++) {
    buf[1 + i] = static_cast<uint8_t>(orderRefNum >> (56 - i * 8));
  }
  return buf;
}

inline std::vector<uint8_t> buildOrderExecutedMessage(uint64_t orderRefNum,uint32_t executedShares){
  std::vector<uint8_t> buf(13, 0);
  buf[0] = 'E';
  for (int i = 0; i < 8; i++) {
    buf[1 + i] = static_cast<uint8_t>(orderRefNum >> (56 - i * 8));
  }

  for (int i = 0; i < 4; i++) {
    buf[9 + i] = static_cast<uint8_t>(executedShares >> (24 - i * 8));
  }

  return buf;
}

inline std::vector<uint8_t> buildOrderCancelMessage(uint64_t orderRefNum,uint32_t cancelledShares) {
  std::vector<uint8_t> buf(13, 0);
  buf[0] = 'X';
  for (int i = 0; i < 8; i++) {
    buf[1 + i] = static_cast<uint8_t>(orderRefNum >> (56 - i * 8));
  }

  for (int i = 0; i < 4; i++) {
    buf[9 + i] = static_cast<uint8_t>(cancelledShares >> (24 - i * 8));
  }

  return buf;
}

inline std::vector<uint8_t> buildOrderTradeMessage(uint64_t orderRefNum,
    uint32_t shares,
    const char* symbol,
    uint32_t price) {
  std::vector<uint8_t> buf(25, 0);
  buf[0] = 'P';
  for (int i = 0; i < 8; i++) {
    buf[1 + i] = static_cast<uint8_t>(orderRefNum >> (56 - i * 8));
  }

  for (int i = 0; i < 4; i++) {
    buf[9 + i] = static_cast<uint8_t>(shares >> (24 - i * 8));
  }

  for (int i = 0; i < 8; i++) {
    buf[13 + i] = (i < static_cast<int>(strlen(symbol))) ? symbol[i] : ' ';
  }

  for (int i = 0; i < 4; i++) {
    buf[21 + i] = static_cast<uint8_t>(price >> (24 - i * 8));
  }

  return buf;
}