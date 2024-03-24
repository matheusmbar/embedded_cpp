#pragma once

#include "devices/lcd/ssd1306.hpp"
#include "snake.hpp"

class SnakeUi {
 public:
  SnakeUi(const Snake& snake, SSD1306& lcd) : snake_(snake), lcd_(lcd) {}

  void DrawFruit(const Position& fruit);
  void DrawBody();

 private:
  const Snake& snake_;
  SSD1306& lcd_;
  static constexpr uint8_t grid_size_{6};
  static constexpr uint8_t grid_half_{grid_size_ / 2};

  static constexpr uint8_t fruit_size_{4};
  static constexpr uint8_t fruit_half_{fruit_size_ / 2};

  static constexpr uint8_t body_size_{4};
  static constexpr uint8_t body_half_{body_size_ / 2};

  Position PosOnGrid(Position pos) const;
};
