// Based of Graphics library by ladyada/Elegoo with init code from Rossum
// MIT license

#include "tft.h"
// TODO: remove pico specific dependency
#include "pico/stdlib.h"
#include "register.h"

#include <stdint.h>
#include <stdio.h>

TFT::TFT(tftInterface *tft_interface)
    : tft_interface(tft_interface), GFX(TFTWIDTH, TFTHEIGHT) {}

void TFT::init() {
  rotation = 0;
  setRotation(rotation);
  tft_interface->writeRegister8(ILI9341_SOFTRESET, 0);
  sleep_ms(50);
  tft_interface->writeRegister8(ILI9341_DISPLAYOFF, 0);

  tft_interface->writeRegister8(ILI9341_POWERCONTROL1, 0x03);
  tft_interface->writeRegister8(ILI9341_POWERCONTROL2, 0x10);
  tft_interface->writeRegister16(ILI9341_VCOMCONTROL1, 0x2B2B);
  tft_interface->writeRegister8(ILI9341_VCOMCONTROL2, 0xC0);
  tft_interface->writeRegister8(ILI9341_MEMCONTROL,
                                ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
  tft_interface->writeRegister8(ILI9341_PIXELFORMAT, 0x55);
  tft_interface->writeRegister16(ILI9341_FRAMECONTROL, 0x001B);

  tft_interface->writeRegister8(ILI9341_ENTRYMODE, 0x07);
  // tft_interface->writeRegister32(ILI9341_DISPLAYFUNC, 0x0A822700);
  tft_interface->writeRegister8(0xbf, 0x7);
  tft_interface->writeRegister8(0xbe, 0xff);

  tft_interface->writeRegister8(ILI9341_SLEEPOUT, 0);
  sleep_ms(150);
  tft_interface->writeRegister8(ILI9341_DISPLAYON, 0);
  sleep_ms(500);
  setAddrWindow(0, 0, _width - 1, _height - 1);
}

void TFT::drawPixel(int16_t x, int16_t y, uint16_t color) {

  // Clip
  if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
    return;

  setAddrWindow(x, y, _width - 1, _height - 1);

  tft_interface->write(0x2c, 1, color, 2);
}

void TFT::setRotation(uint8_t rotation) {
  uint16_t t;

  GFX::setRotation(rotation);

  switch (rotation) {
  case 2:
    t = ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR;
    break;
  case 3:
    t = ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
    break;
  case 0:
    t = ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
    break;
  case 1:
    t = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV |
        ILI9341_MADCTL_BGR;
    break;
  }
  tft_interface->writeRegister8(ILI9341_MADCTL, t); // MADCTL
  setAddrWindow(0, 0, _width - 1, _height - 1);     // CS_IDLE happens here
}

void TFT::setAddrWindow(int x1, int y1, int x2, int y2) {
  uint32_t t;

  t = x1;
  t <<= 16;
  t |= x2;
  tft_interface->writeRegister32(ILI9341_COLADDRSET,
                                 t); // HX8357D uses same registers!
  t = y1;
  t <<= 16;
  t |= y2;
  tft_interface->writeRegister32(ILI9341_PAGEADDRSET,
                                 t); // HX8357D uses same registers!
}

void TFT::flood(uint16_t color, uint32_t len) {
  uint16_t blocks;
  uint8_t i, hi = color >> 8, lo = color;

  tft_interface->write(0x2C, 1, color, 2);

  len--;

  blocks = (uint16_t)(len / 64); // 64 pixels/block

  // TODO: Pending interface decision to support block write
  // if (hi == lo) {
  //   // High and low bytes are identical.  Leave prior data
  //   // on the port(s) and just toggle the write strobe.
  //   while (blocks--) {
  //     i = 16; // 64 pixels/block / 4 pixels/pass
  //     do {
  //       WR_STROBE;
  //       WR_STROBE;
  //       WR_STROBE;
  //       WR_STROBE; // 2 bytes/pixel
  //       WR_STROBE;
  //       WR_STROBE;
  //       WR_STROBE;
  //       WR_STROBE; // x 4 pixels
  //     } while (--i);
  //   }
  //   // Fill any remaining pixels (1 to 64)
  //   for (i = (uint8_t)len & 63; i--;) {
  //     WR_STROBE;
  //     WR_STROBE;
  //   }
  // } else {

  while (blocks--) {
    i = 16; // 64 pixels/block / 4 pixels/pass
    do {
      tft_interface->write8data(hi);
      tft_interface->write8data(lo);
      tft_interface->write8data(hi);
      tft_interface->write8data(lo);
      tft_interface->write8data(hi);
      tft_interface->write8data(lo);
      tft_interface->write8data(hi);
      tft_interface->write8data(lo);
    } while (--i);
  }
  for (i = (uint8_t)len & 63; i--;) {
    tft_interface->write8data(hi);
    tft_interface->write8data(lo);
  }
  tft_interface->set_cs(true);

  tft_interface->set_cs(false);
  // }
}

void TFT::fillScreen(uint16_t color) {

  setAddrWindow(0, 0, _width - 1, _height - 1);

  flood(color, (long)TFTWIDTH * (long)TFTHEIGHT);
}

void TFT::sleep(bool action) {
  if (action)
    tft_interface->writeRegister8(ILI9341_DISPLAYOFF, action);
  else
    tft_interface->writeRegister8(ILI9341_DISPLAYON, action);
  sleep_ms(200);
}

void TFT::setBacklight(uint8_t brightness) {
  tft_interface->writeRegister8(0x51, brightness);
}