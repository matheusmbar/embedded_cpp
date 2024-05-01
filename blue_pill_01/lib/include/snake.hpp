#pragma once

#include <etl/random.h>
#include <etl/vector.h>

#include <cstdint>

struct Position {
  explicit Position(uint8_t x = 0, uint8_t y = 0) : x_(x), y_(y) {}

  [[nodiscard]] constexpr uint8_t x() const {
    return x_;
  }

  [[nodiscard]] constexpr uint8_t y() const {
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
  enum class Action { kUp, kDown, kLeft, kRight, kEnter, kNone };

  Snake();

  void Reset();

  bool ProcessFruit(const Position fruit);

  void ProcessAction(Snake::Action action);

  [[nodiscard]] bool ColisionDetected() const;

  [[nodiscard]] const Position& GetHead() const;

  [[nodiscard]] const etl::ivector<Position>& GetBody() const;

  [[nodiscard]] int16_t GetPoints() const;

  Position GetFruit();

 private:
  static constexpr uint8_t startLen = 5;
  static constexpr uint8_t kStep = 1;
  static constexpr uint8_t kPosX = 20;
  static constexpr uint8_t kPosY = 9;
  static constexpr uint8_t tailStartX = 10;
  static constexpr uint8_t tailStartY = 5;
  static_assert(tailStartX < kPosX);
  static_assert(tailStartY < kPosY);

  static constexpr uint8_t kTotalPos = kPosX * kPosY;

  etl::vector<Position, Snake::kTotalPos> body;
  Position new_head;

  Action last_action{Action::kLeft};
  uint16_t points{0};
  bool self_colision{false};
  bool ate_fruit{false};
  etl::random_xorshift random;

  void CalculateNewHead(Snake::Action action);

  void CheckColision();
};
