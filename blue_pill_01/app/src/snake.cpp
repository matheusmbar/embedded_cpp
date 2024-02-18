#include "snake.hpp"

Snake::Snake() {
  Reset();
}

void Snake::Reset() {
  last_action = Action::kLeft;
  self_colision = false;
  ate_fruit = false;
  points = 0;

  body.clear();
  const uint8_t tailStartX = 60;
  const uint8_t tailStartY = 30;
  const uint8_t startLen = 5;
  for (uint8_t i = 0; i < startLen; i++) {
    uint8_t next_x = kStep * i + tailStartX;
    body.push_back(etl::array<uint8_t, 2>({next_x, tailStartY}));
  }
}

bool Snake::ProcessFruit(const etl::array<uint8_t, 2>& fruit) {
  ate_fruit = (GetHead() == fruit);
  if (ate_fruit) {
    ++points;
  } else {
    body.pop_back();
  }
  return ate_fruit;
}

void Snake::ProcessAction(Snake::Action action) {
  CalculateNewHead(action);
  CheckColision();

  if (!self_colision) {
    body.insert(body.begin(), new_head);
  }
}

bool Snake::ColisionDetected() const {
  return self_colision;
}

const etl::array<uint8_t, 2>& Snake::GetHead() const {
  return *body.begin();
}

const etl::ivector<etl::array<uint8_t, 2>>& Snake::GetBody() const {
  return body;
}

int16_t Snake::GetPoints() const {
  return points;
}

void Snake::CalculateNewHead(Snake::Action action) {
  int16_t new_x = GetHead()[0];
  int16_t new_y = GetHead()[1];

  switch (action) {
    case Action::kUp:
      if (last_action == Action::kDown) {
        action = Action::kDown;
      }
      break;
    case Action::kDown:
      if (last_action == Action::kUp) {
        action = Action::kUp;
      }
      break;
    case Action::kLeft:
      if (last_action == Action::kRight) {
        action = Action::kRight;
      }
      break;
    case Action::kRight:
      if (last_action == Action::kLeft) {
        action = Action::kLeft;
      }
      break;
    case Action::kNone:
      action = last_action;
      break;
  }

  switch (action) {
    case Action::kUp:
      new_y -= kStep;
      break;
    case Action::kDown:
      new_y += kStep;
      break;
    case Action::kLeft:
      new_x -= kStep;
      break;
    case Action::kRight:
      new_x += kStep;
      break;
    default:
      break;
  }

  if (new_x < kMinX) {
    new_x = kMaxX;
  } else if (new_x > kMaxX) {
    new_x = kMinX;
  }

  if (new_y < kMinY) {
    new_y = kMaxY;
  } else if (new_y > kMaxY) {
    new_y = kMinY;
  }
  last_action = action;

  new_head = etl::array<uint8_t, 2>({static_cast<uint8_t>(new_x), static_cast<uint8_t>(new_y)});
}

void Snake::CheckColision() {
  self_colision = false;
  if (etl::find(body.begin(), body.end(), new_head) != body.end()) {
    self_colision = true;
  }
}
