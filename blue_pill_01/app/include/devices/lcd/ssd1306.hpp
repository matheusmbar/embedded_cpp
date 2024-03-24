#pragma once
#include <etl/string.h>
#include <etl/vector.h>

#include <cstdint>
#include <memory>

#include "devices/keypad/keypad_interface.hpp"

#ifndef NDEBUG
#include "u8g2.h"
#endif

class SSD1306 {
 public:
  explicit SSD1306(uint32_t i2c_bus);
  explicit SSD1306(uint32_t i2c_bus, std::shared_ptr<KeypadInterface> keypad);
  ~SSD1306();

  enum class Font { k5x7_Tn, k7x14B_Tr };

  void Refresh();

  void ClearBuffer();

  void ClearDisplay();

  void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

  void DrawCircle(uint16_t x0, uint16_t y0, uint16_t rad);

  uint16_t DrawStr(uint16_t x, uint16_t y, const etl::istring& text);

  void SetFont(Font font);

  uint8_t UiMessage(const etl::istring& title_1, const etl::istring& title_2,
                    const etl::istring& title_3, const etl::istring& buttons_list);
  bool UiInputValue(const etl::istring& title, uint8_t min, uint8_t max, uint8_t& value);
  bool UiSelectionList(const etl::istring& title, uint8_t& current_pos,
                       const etl::istring& list_items);

#ifndef NDEBUG
  u8g2_t* GetU8g2();
#endif

 private:
  struct SSD1306Impl;
  std::unique_ptr<SSD1306Impl> pImpl_;
};
