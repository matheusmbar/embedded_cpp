#include "snake_ui.hpp"

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

Position SnakeUi::PosOnGrid(Position pos) const {
  uint8_t x = grid_half_ + pos.x() * grid_size_;
  uint8_t y = grid_half_ + pos.y() * grid_size_;
  return Position(x, y);
}
