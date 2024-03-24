#pragma once

#include <etl/vector.h>

#include <cstdint>

struct Position {
  explicit Position(uint8_t x = 0, uint8_t y = 0) : x_(x), y_(y) {}

  constexpr uint8_t x() const {
    return x_;
  }

  constexpr uint8_t y() const {
    return y_;
  }

 private:
  uint8_t x_;
  uint8_t y_;
};

constexpr bool operator==(const Position& lhs, const Position& rhs) {
  return (lhs.x() == rhs.x()) && (lhs.y() == rhs.y());
}
constexpr bool operator!=(const Position& lhs, const Position& rhs) {
  return !(lhs == rhs);
}

class Snake {
 public:
  static const uint8_t kStep = 6;
  static const uint8_t kMinX = 1 * kStep;
  static const uint8_t kMinY = 1 * kStep;

  enum class Action { kUp, kDown, kLeft, kRight, kNone };

  Snake();

  void Reset();

  bool ProcessFruit(const Position fruit);

  void ProcessAction(Snake::Action action);

  bool ColisionDetected() const;

  const Position& GetHead() const;

  const etl::ivector<Position>& GetBody() const;

  int16_t GetPoints() const;

 private:
  static const uint8_t kPosX = 20;
  static const uint8_t kPosY = 10;
  static const uint8_t kMaxX = (kPosX - 1) * kStep + kMinX;
  static const uint8_t kMaxY = (kPosY - 1) * kStep + kMinY;
  static const uint8_t kTotalPos = kPosX * kPosY;

  static_assert((kMaxX - kMinX) % kStep == 0);
  static_assert((kMaxY - kMinY) % kStep == 0);

  etl::vector<Position, Snake::kTotalPos> body;
  Position new_head;

  Action last_action{Action::kLeft};
  uint16_t points{0};
  bool self_colision{false};
  bool ate_fruit{false};

  void CalculateNewHead(Snake::Action action);

  void CheckColision();
};
