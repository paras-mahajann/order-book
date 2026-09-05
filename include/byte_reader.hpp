#pragma once
#include<cstdint>
#include<cstddef>
#include<array>

inline uint32_t readBigEndian32(const uint8_t* bytes) { 
	return (static_cast<uint32_t>(bytes[0]) << 24) |
         (static_cast<uint32_t>(bytes[1]) << 16) |
         (static_cast<uint32_t>(bytes[2]) << 8) |
         (static_cast<uint32_t>(bytes[3]));
}

inline uint64_t readBigEndian64(const uint8_t* bytes) {
  uint64_t result = 0;
  for (int i = 0; i < 8; i++) {
      result = (result << 8) | bytes[i];
  }
  return result;
}

inline std::array<char, 8> readSymbol(const uint8_t* bytes) {
  std::array<char, 8> symbol;
  for (int i = 0; i < 8; i++) {
    symbol[i] = static_cast<char>(bytes[i]);
  }
  return symbol;
}