#include "snake_ui.hpp"

#include <etl/string.h>
#include <etl/to_string.h>

void SnakeUi::DrawFruit(const Position& fruit) {
  auto pos = PosOnGrid(fruit);

  const uint16_t x_max = pos.x() + fruit_half_;
  const uint16_t x_min = pos.x() - fruit_half_;
  const uint16_t y_max = pos.y() + fruit_half_;
  const uint16_t y_min = pos.y() - fruit_half_;

  lcd_.DrawLine(x_min, y_max, x_max, y_min);
  lcd_.DrawLine(x_min, y_min, x_max, y_max);
}

void SnakeUi::DrawBody() {
  for (const auto& section : snake_.GetBody()) {
    auto pos = PosOnGrid(section);
    lcd_.DrawCircle(pos.x(), pos.y(), body_half_);
  }
  const auto head = PosOnGrid(snake_.GetHead());
  lcd_.DrawCircle(head.x(), head.y(), 1);
}

void SnakeUi::DrawGame(const Position& fruit, int points) {
  lcd_.ClearBuffer();

  lcd_.SetFont(SSD1306::Font::k5x7_Tn);
  etl::to_string<size_t>(points, msg_);
  lcd_.DrawStr(105, 15, msg_);

  DrawBody();
  DrawFruit(fruit);
  lcd_.Refresh();
}

void SnakeUi::DrawGameOver(int points, int max_points) {
  lcd_.ClearBuffer();
  lcd_.SetFont(SSD1306::Font::k7x14B_Tr);
  msg_ = "GAME OVER";
  lcd_.DrawStr(30, 20, msg_);
  etl::to_string<int>(points, msg_);
  lcd_.DrawStr(60, 40, msg_);
  if (points == max_points) {
    msg_ = "New record!";
  } else {
    msg_ = "Record: ";
    etl::to_string<int>(max_points, msg_, true);
  }
  lcd_.DrawStr(28, 60, msg_);
  lcd_.Refresh();
}

Position SnakeUi::PosOnGrid(Position pos) const {
  uint8_t x = grid_half_ + pos.x() * grid_size_;
  uint8_t y = grid_half_ + pos.y() * grid_size_;
  return Position(x, y);
}

void SnakeUi::Reset() {
  lcd_.ClearDisplay();
}

Snake::Action SnakeUi::GetAction() {
  auto& pressed = keypad_->GetPressed();
  if (pressed.empty()) {
    return Snake::Action::kNone;
  }

  if (pressed[0] == Key::kUp) {
    return Snake::Action::kUp;
  }

  if (pressed[0] == Key::kDown) {
    return Snake::Action::kDown;
  }

  if (pressed[0] == Key::kLeft) {
    return Snake::Action::kLeft;
  }

  if (pressed[0] == Key::kRight) {
    return Snake::Action::kRight;
  }

  if (pressed[0] == Key::kCenter) {
    return Snake::Action::kEnter;
  }

  return Snake::Action::kNone;
}
