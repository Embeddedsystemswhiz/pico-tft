// Based of Graphics library by ladyada/Elegoo with init code from Rossum
// MIT license

#pragma once

#include "../interfaces/tft_interface.h"
#include "../libs/gfx.h"

#define TFTWIDTH 240
#define TFTHEIGHT 320

class TFT : public GFX {
  tftInterface *tft_interface;

public:
  TFT(tftInterface *tft_interface);
  void init();
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void setRotation(uint8_t rotation);
  void setAddrWindow(int x1, int y1, int x2, int y2);
  void flood(uint16_t color, uint32_t len);
  void fillScreen(uint16_t color);
  void sleep(bool action);
  void setBacklight(uint8_t brightness);
};