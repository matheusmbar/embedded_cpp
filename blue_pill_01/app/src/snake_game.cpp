#include "snake_game.hpp"

void SnakeGame::Setup() {
  snake_->Reset();
  ui_->Reset();
}

void SnakeGame::Run() {
  auto fruit = snake_->GetFruit();

  while (!snake_->ColisionDetected()) {
    ui_->DrawGame(fruit, points_);
    if (snake_->ProcessFruit(fruit)) {
      fruit = snake_->GetFruit();
    }
    points_ = snake_->GetPoints();

    auto btn_action{Snake::Action::kNone};

    for (auto i = 0; i < 10; i++) {
      if (auto action = ui_->GetAction(); action != Snake::Action::kNone) {
        btn_action = action;
      }
      int wait_ms = 20 - points_;
      wait_ms = wait_ms < 1 ? 1 : wait_ms;
      delay_ms_(wait_ms);
    }
    snake_->ProcessAction(btn_action);
  }

  while (ui_->GetAction() != Snake::Action::kEnter) {
    delay_ms_(50);
  }

  if (points_ > max_points_) {
    max_points_ = points_;
  }
  ui_->DrawGameOver(points_, max_points_);

  while (ui_->GetAction() == Snake::Action::kEnter) {
    delay_ms_(50);
  }
  while (ui_->GetAction() != Snake::Action::kEnter) {
    delay_ms_(50);
  }

  Setup();
}

bool SnakeGame::Finished() {
  return false;
}
