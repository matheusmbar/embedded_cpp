#include "devices/lcd/ssd1306.hpp"

#include <etl/byte.h>
#include <etl/string.h>
#include <etl/vector.h>
#include <libopencm3/stm32/i2c.h>

#include "u8g2.h"

struct SSD1306::SSD1306Impl {
  static uint32_t i2c_bus_;
  static std::shared_ptr<GpioInterface> menu_select_;
  static std::shared_ptr<GpioInterface> menu_home_;
  static std::shared_ptr<GpioInterface> menu_next_;
  static std::shared_ptr<GpioInterface> menu_prev_;
  static std::shared_ptr<GpioInterface> menu_up_;
  static std::shared_ptr<GpioInterface> menu_down_;

  u8g2_t u8g2;
  SSD1306Impl() = default;

  static uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void const *arg_ptr) {
    // TODO(matheusmbar) Implement a way to call a non-static method from here,
    // allowing multiple displays, each with its own buffer and I2C device/address

    /* u8g2/u8x8 will never send more than 32 bytes between
                                      START_TRANSFER and END_TRANSFER */
    static etl::vector<uint8_t, 32> buffer;
    uint8_t const *data;

    switch (msg) {
      case U8X8_MSG_BYTE_START_TRANSFER:
        buffer.clear();
        break;
      case U8X8_MSG_BYTE_SEND:
        data = static_cast<uint8_t const *>(arg_ptr);
        for (; arg_int > 0; --arg_int, ++data) {
          buffer.push_back(*data);
        }
        break;
      case U8X8_MSG_BYTE_END_TRANSFER: {
        etl::byte i2c_addr{u8x8_GetI2CAddress(u8x8)};

        i2c_transfer7(i2c_bus_, etl::to_integer<uint8_t>(i2c_addr >> 1), buffer.data(),
                      buffer.size(), nullptr, 0);
        break;
      }
      case U8X8_MSG_BYTE_INIT:
      case U8X8_MSG_BYTE_SET_DC:
        break;
      default:
        return 0;
    }
    return 1;
  }

  static uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    return u8x8_byte_hw_i2c(u8x8, msg, arg_int, static_cast<void const *>(arg_ptr));
  }

  static uint8_t gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t, void *) {
    bool pressed = false;
    switch (msg) {
      case U8X8_MSG_GPIO_MENU_SELECT:
        pressed = menu_select_ && (menu_select_->Get() == GpioState::kHigh);
        break;
      case U8X8_MSG_GPIO_MENU_NEXT:
        pressed = menu_next_ && (menu_next_->Get() == GpioState::kHigh);
        break;
      case U8X8_MSG_GPIO_MENU_PREV:
        pressed = menu_prev_ && (menu_prev_->Get() == GpioState::kHigh);
        break;
      case U8X8_MSG_GPIO_MENU_HOME:
        pressed = menu_home_ && (menu_home_->Get() == GpioState::kHigh);
        break;
      case U8X8_MSG_GPIO_MENU_UP:
        pressed = menu_up_ && (menu_up_->Get() == GpioState::kHigh);
        break;
      case U8X8_MSG_GPIO_MENU_DOWN:
        pressed = menu_down_ && (menu_down_->Get() == GpioState::kHigh);
        break;
      default:
        break;
    }
    // Report buttons as active HIGH
    u8x8->gpio_result = pressed ? 0 : 1;

    return 1;
  }
};

uint32_t SSD1306::SSD1306Impl::i2c_bus_ = 0;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_select_;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_home_;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_next_;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_prev_;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_up_;
std::shared_ptr<GpioInterface> SSD1306::SSD1306Impl::menu_down_;

SSD1306::SSD1306(uint32_t i2c_bus) : pImpl_(std::make_unique<SSD1306Impl>()) {
  SSD1306Impl::i2c_bus_ = i2c_bus;

  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&pImpl_->u8g2, U8G2_R0, SSD1306Impl::u8x8_byte_hw_i2c,
                                         SSD1306Impl::gpio_and_delay_cb);
  u8g2_InitDisplay(&pImpl_->u8g2);
  u8g2_SetPowerSave(&pImpl_->u8g2, 0);  // wake up display
  u8g2_SetFont(&pImpl_->u8g2, u8g2_font_ncenR10_tf);

  // Set Buttons as active HIGH
  pImpl_->u8g2.u8x8.debounce_default_pin_state = 0;
}

SSD1306::SSD1306(uint32_t i2c_bus, std::shared_ptr<GpioInterface> menu_select,
                 std::shared_ptr<GpioInterface> menu_home, std::shared_ptr<GpioInterface> menu_next,
                 std::shared_ptr<GpioInterface> menu_prev, std::shared_ptr<GpioInterface> menu_up,
                 std::shared_ptr<GpioInterface> menu_down)
    : pImpl_(std::make_unique<SSD1306Impl>()) {
  SSD1306Impl::menu_select_ = menu_select;
  SSD1306Impl::menu_home_ = menu_home;
  SSD1306Impl::menu_next_ = menu_next;
  SSD1306Impl::menu_prev_ = menu_prev;
  SSD1306Impl::menu_up_ = menu_up;
  SSD1306Impl::menu_down_ = menu_down;

  SSD1306Impl::i2c_bus_ = i2c_bus;

  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&pImpl_->u8g2, U8G2_R0, SSD1306Impl::u8x8_byte_hw_i2c,
                                         SSD1306Impl::gpio_and_delay_cb);
  u8g2_InitDisplay(&pImpl_->u8g2);
  u8g2_SetPowerSave(&pImpl_->u8g2, 0);  // wake up display
  u8g2_SetFont(&pImpl_->u8g2, u8g2_font_ncenR10_tf);
}

#ifndef NDEBUG
u8g2_t *SSD1306::GetU8g2() {
  return &pImpl_->u8g2;
}
#endif

SSD1306::~SSD1306() = default;

void SSD1306::Refresh() {
  u8g2_SendBuffer(&pImpl_->u8g2);
}

void SSD1306::ClearDisplay() {
  u8g2_ClearDisplay(&pImpl_->u8g2);
}

void SSD1306::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  u8g2_DrawLine(&pImpl_->u8g2, x1, y1, x2, y2);
}

void SSD1306::DrawBox(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  u8g2_DrawBox(&pImpl_->u8g2, x, y, width, height);
}

void SSD1306::DrawCircle(uint16_t x0, uint16_t y0, uint16_t rad) {
  u8g2_DrawCircle(&pImpl_->u8g2, x0, y0, rad, U8G2_DRAW_ALL);
}

uint16_t SSD1306::DrawStr(uint16_t x, uint16_t y, const etl::istring &text) {
  return u8g2_DrawStr(&pImpl_->u8g2, x, y, text.c_str());
}

uint8_t SSD1306::UiMessage(const etl::istring &title_1, const etl::istring &title_2,
                           const etl::istring &title_3, const etl::istring &buttons_list) {
  return u8g2_UserInterfaceMessage(&pImpl_->u8g2, title_1.c_str(), title_2.c_str(), title_3.c_str(),
                                   buttons_list.c_str());
}

bool SSD1306::UiInputValue(const etl::istring &title, uint8_t min, uint8_t max, uint8_t &value) {
  bool value_min_max = false;
  if (value < min) {
    value = min;
    value_min_max = true;
  } else if (value > max) {
    value = max;
    value_min_max = true;
  }
  bool value_changed =
      u8g2_UserInterfaceInputValue(&pImpl_->u8g2, title.c_str(), "", &value, min, max, 3, "");
  return value_changed || value_min_max;
}

bool SSD1306::UiSelectionList(const etl::istring &title, uint8_t &current_pos,
                              const etl::istring &list_items) {
  if (uint8_t new_pos = u8g2_UserInterfaceSelectionList(&pImpl_->u8g2, title.c_str(), current_pos,
                                                        list_items.c_str());
      new_pos) {
    current_pos = new_pos;
    return true;
  }
  return false;
}
