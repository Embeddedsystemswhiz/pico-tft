#pragma once

#include "../interfaces/tft_interface.h"
#include "../libs/elegoo_gfx.h"

#define TFTWIDTH 240
#define TFTHEIGHT 320

class elegoo_tft : public elegoo_gfx {
  tft_interface *tft_interface_inst;

public:
  elegoo_tft(tft_interface *tft_interface);
  void init();
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void set_rotation(uint8_t rotation);
  void setAddrWindow(int x1, int y1, int x2, int y2);
  void flood(uint16_t color, uint32_t len);
  void fillScreen(uint16_t color);
};