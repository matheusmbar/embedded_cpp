#pragma once
#include <etl/string.h>
#include <etl/vector.h>

#include <cstdint>
#include <memory>

#include "gpio/gpio_interface.hpp"

class SSD1306 {
 public:
  explicit SSD1306(uint32_t i2c_bus);
  explicit SSD1306(uint32_t i2c_bus, std::shared_ptr<GpioInterface> menu_select,
                   std::shared_ptr<GpioInterface> menu_home,
                   std::shared_ptr<GpioInterface> menu_next,
                   std::shared_ptr<GpioInterface> menu_prev, std::shared_ptr<GpioInterface> menu_up,
                   std::shared_ptr<GpioInterface> menu_down);
  ~SSD1306();

  void Refresh();

  void ClearDisplay();

  void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  void DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

  void DrawCircle(uint16_t x0, uint16_t y0, uint16_t rad);

  uint16_t DrawStr(uint16_t x, uint16_t y, const etl::istring& text);

  uint8_t UiMessage(const etl::istring& title_1, const etl::istring& title_2,
                    const etl::istring& title_3, const etl::istring& buttons_list);
  bool UiInputValue(const etl::istring& title, uint8_t min, uint8_t max, uint8_t& value);
  bool UiSelectionList(const etl::istring& title, uint8_t& current_pos,
                       const etl::istring& list_items);

 private:
  struct SSD1306Impl;
  std::unique_ptr<SSD1306Impl> pImpl_;
};
