#pragma once
#include <etl/string.h>

#include <cstdint>
#include <memory>

class SSD1306 {
 public:
  explicit SSD1306(uint32_t i2c_bus);
  ~SSD1306();

  void Refresh();

  void ClearDisplay();

  void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

  void DrawCircle(uint16_t x0, uint16_t y0, uint16_t rad);

  uint16_t DrawStr(uint16_t x, uint16_t y, const etl::istring& text);

 private:
  struct SSD1306Impl;
  std::unique_ptr<SSD1306Impl> pImpl_;
};
