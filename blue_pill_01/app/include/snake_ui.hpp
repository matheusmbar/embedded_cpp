#pragma once

#include <etl/string.h>

#include <memory>

#include "devices/keypad/keypad_interface.hpp"
#include "devices/lcd/ssd1306.hpp"
#include "snake.hpp"

class SnakeUi {
 public:
  SnakeUi(const std::shared_ptr<Snake> snake, SSD1306& lcd, std::shared_ptr<KeypadInterface> keypad)
      : snake_(snake), lcd_(lcd), keypad_(keypad) {}

  void DrawFruit(const Position& fruit);
  void DrawBody();
  void DrawGame(const Position& fruit, int points);
  void DrawGameOver(int points, int max_points);
  void Reset();

  Snake::Action GetAction();

 private:
  const std::shared_ptr<Snake> snake_;
  SSD1306& lcd_;
  static constexpr uint8_t grid_size_{6};
  static constexpr uint8_t grid_half_{grid_size_ / 2};

  static constexpr uint8_t fruit_size_{4};
  static constexpr uint8_t fruit_half_{fruit_size_ / 2};

  static constexpr uint8_t body_size_{4};
  static constexpr uint8_t body_half_{body_size_ / 2};

  std::shared_ptr<KeypadInterface> keypad_;
  etl::string<15> msg_{};

  Position PosOnGrid(Position pos) const;
};
