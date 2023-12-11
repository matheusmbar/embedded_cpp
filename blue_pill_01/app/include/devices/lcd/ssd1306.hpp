#pragma once

#include <etl/array.h>
#include <etl/array_view.h>
#include <etl/byte.h>
#include <etl/vector.h>
#include <libopencm3/stm32/i2c.h>

/// fit into the SSD1306_ naming scheme
constexpr uint8_t SSD1306_BLACK{0};    ///< Draw 'off' pixels
constexpr uint8_t SSD1306_WHITE{1};    ///< Draw 'on' pixels
constexpr uint8_t SSD1306_INVERSE{2};  ///< Invert pixels

constexpr uint8_t SSD1306_MEMORYMODE{0x20};
constexpr uint8_t SSD1306_COLUMNADDR{0x21};
constexpr uint8_t SSD1306_PAGEADDR{0x22};
constexpr uint8_t SSD1306_SETCONTRAST{0x81};
constexpr uint8_t SSD1306_CHARGEPUMP{0x8D};
constexpr uint8_t SSD1306_SEGREMAP{0xA0};
constexpr uint8_t SSD1306_DISPLAYALLON_RESUME{0xA4};
constexpr uint8_t SSD1306_DISPLAYALLON{0xA5};
constexpr uint8_t SSD1306_NORMALDISPLAY{0xA6};
constexpr uint8_t SSD1306_INVERTDISPLAY{0xA7};
constexpr uint8_t SSD1306_SETMULTIPLEX{0xA8};
constexpr uint8_t SSD1306_DISPLAYOFF{0xAE};
constexpr uint8_t SSD1306_DISPLAYON{0xAF};
constexpr uint8_t SSD1306_COMSCANINC{0xC0};
constexpr uint8_t SSD1306_COMSCANDEC{0xC8};
constexpr uint8_t SSD1306_SETDISPLAYOFFSET{0xD3};
constexpr uint8_t SSD1306_SETDISPLAYCLOCKDIV{0xD5};
constexpr uint8_t SSD1306_SETPRECHARGE{0xD9};
constexpr uint8_t SSD1306_SETCOMPINS{0xDA};
constexpr uint8_t SSD1306_SETVCOMDETECT{0xDB};

constexpr uint8_t SSD1306_SETLOWCOLUMN{0x00};
constexpr uint8_t SSD1306_SETHIGHCOLUMN{0x10};
constexpr uint8_t SSD1306_SETSTARTLINE{0x40};

constexpr uint8_t SSD1306_EXTERNALVCC{0x01};   ///< External display voltage source
constexpr uint8_t SSD1306_SWITCHCAPVCC{0x02};  ///< Gen. display voltage from 3.3V

constexpr uint8_t SSD1306_CONTROL_COMMAND{0x00};  ///< Next byte is a command
constexpr uint8_t SSD1306_CONTROL_DATA{0x40};     ///< Next byte is data for GDDRAM

constexpr uint8_t SSD1306_RIGHT_HORIZONTAL_SCROLL{0x26};               ///< Init rt scroll
constexpr uint8_t SSD1306_LEFT_HORIZONTAL_SCROLL{0x27};                ///< Init left scroll
constexpr uint8_t SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL{0x29};  ///< Init diag scroll
constexpr uint8_t SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL{0x2A};   ///< Init diag scroll
constexpr uint8_t SSD1306_DEACTIVATE_SCROLL{0x2E};                     ///< Stop scroll
constexpr uint8_t SSD1306_ACTIVATE_SCROLL{0x2F};                       ///< Start scroll
constexpr uint8_t SSD1306_SET_VERTICAL_SCROLL_AREA{0xA3};              ///< Set scroll range

template <const uint16_t width, const uint16_t height>
class SSD1306 {
 public:
  explicit SSD1306(uint32_t i2c, uint8_t addr) : i2c_(i2c), addr_(addr) {
    buffer_.assign(buffer_.max_size(), 0);
  }

  void Initialize() {
    const uint8_t vccstate{SSD1306_SWITCHCAPVCC};

    const etl::vector<uint8_t, 5> init1 = {SSD1306_CONTROL_COMMAND, SSD1306_DISPLAYOFF,  // 0xAE
                                           SSD1306_SETDISPLAYCLOCKDIV,                   // 0xD5
                                           0x80,                   // the suggested ratio 0x80
                                           SSD1306_SETMULTIPLEX};  // 0xA8
    const etl::vector<uint8_t, 5> init2 = {SSD1306_CONTROL_COMMAND,
                                           SSD1306_SETDISPLAYOFFSET,    // 0xD3
                                           0x0,                         // no offset
                                           SSD1306_SETSTARTLINE | 0x0,  // line #0
                                           SSD1306_CHARGEPUMP};         // 0x8D

    const etl::vector<uint8_t, 5> init3 = {SSD1306_CONTROL_COMMAND, SSD1306_MEMORYMODE,  // 0x20
                                           0x00,  // 0x0 act like ks0108
                                           SSD1306_SEGREMAP | 0x1, SSD1306_COMSCANDEC};
    const etl::vector<uint8_t, 7> init5 = {SSD1306_CONTROL_COMMAND,
                                           SSD1306_SETVCOMDETECT,  // 0xDB
                                           0x00,
                                           SSD1306_DISPLAYALLON_RESUME,  // 0xA4
                                           SSD1306_NORMALDISPLAY,        // 0xA6
                                           SSD1306_DEACTIVATE_SCROLL,
                                           SSD1306_DISPLAYON};  // Main screen turn on
    const uint8_t com_pins = 0x12;
    const uint8_t contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;

    SendCommandVec(init1);
    SendCommand(height_ - 1);
    SendCommandVec(init2);
    SendCommand((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);
    SendCommandVec(init3);
    SendCommand(SSD1306_SETCOMPINS);
    SendCommand(com_pins);
    SendCommand(SSD1306_SETCONTRAST);
    SendCommand(contrast);
    SendCommand(SSD1306_SETPRECHARGE);
    SendCommand((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
    SendCommandVec(init5);
  }

  void Refresh() {
    const etl::vector<uint8_t, 6> dlist1 = {SSD1306_CONTROL_COMMAND,
                                            SSD1306_PAGEADDR,
                                            0,     // Page start address
                                            0xFF,  // Page end (not really, but works here)
                                            SSD1306_COLUMNADDR,
                                            0};  // Column start address
    SendCommandVec(dlist1);
    SendCommand(width_ - 1);  // Column end address

    buffer_[0] = 0x40;
    SendCommandVec(buffer_);
  }

  void FillWhite() {
    etl::fill(buffer_.begin() + 1, buffer_.end(), 0xFF);
  }

  void FillBlack() {
    etl::fill(buffer_.begin() + 1, buffer_.end(), 0x00);
  }

 private:
  void SendCommandVec(const etl::ivector<uint8_t>& data) const {
    i2c_transfer7(i2c_, addr_, data.data(), data.size(), nullptr, 0);
  }

  void SendCommand(uint8_t value) const {
    const etl::vector<uint8_t, 2> data{0x00, value};
    SendCommandVec(data);
  }

 private:
  const uint32_t i2c_;
  const uint8_t addr_;
  static const uint8_t height_{height};
  static const uint8_t width_{width};
  static const size_t pixels_count_{(height_ * (width_ + 7) / 8)};
  etl::vector<uint8_t, pixels_count_> buffer_;
};
