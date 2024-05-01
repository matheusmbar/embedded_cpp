#pragma once

#include <etl/delegate.h>

#include <memory>

#include "snake_ui.hpp"

class SnakeGame {
 public:
  SnakeGame(std::shared_ptr<Snake> snake, std::unique_ptr<SnakeUi> snake_ui,
            etl::delegate<void(uint32_t)> delay_ms)
      : snake_(std::move(snake)), ui_(std::move(snake_ui)), delay_ms_(delay_ms) {}

  void Setup();
  void Run();
  bool Finished();

 private:
  std::shared_ptr<Snake> snake_;
  std::unique_ptr<SnakeUi> ui_;

  etl::delegate<void(uint32_t)> delay_ms_;
  int points_{0};
  int max_points_{0};
};
