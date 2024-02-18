#pragma once

#include <etl/array.h>
#include <etl/vector.h>

#include <cstdint>

class Snake {
 public:
  static const uint8_t kStep = 6;
  static const uint8_t kMinX = 1 * kStep;
  static const uint8_t kMinY = 1 * kStep;

  enum class Action { kUp, kDown, kLeft, kRight, kNone };

  Snake();

  void Reset();

  bool ProcessFruit(const etl::array<uint8_t, 2>& fruit);

  void ProcessAction(Snake::Action action);

  bool ColisionDetected() const;

  const etl::array<uint8_t, 2>& GetHead() const;

  const etl::ivector<etl::array<uint8_t, 2>>& GetBody() const;

  int16_t GetPoints() const;

 private:
  static const uint8_t kPosX = 20;
  static const uint8_t kPosY = 10;
  static const uint8_t kMaxX = (kPosX - 1) * kStep + kMinX;
  static const uint8_t kMaxY = (kPosY - 1) * kStep + kMinY;
  static const uint8_t kTotalPos = kPosX * kPosY;

  static_assert((kMaxX - kMinX) % kStep == 0);
  static_assert((kMaxY - kMinY) % kStep == 0);

  etl::vector<etl::array<uint8_t, 2>, Snake::kTotalPos> body;
  etl::array<uint8_t, 2> new_head{};

  Action last_action{Action::kLeft};
  uint16_t points{0};
  bool self_colision{false};
  bool ate_fruit{false};

  void CalculateNewHead(Snake::Action action);

  void CheckColision();
};
