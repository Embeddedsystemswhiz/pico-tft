#pragma once

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * @brief Parallel interface to communication to TFT controller
 *        Refer data-sheet
 * here[https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf]
 */
class parallel_interface : public tft_interface {
private:
  static constexpr uint8_t DATA_OFFSET = 0;
  static constexpr uint8_t RST_OFFSET = 8;
  static constexpr uint8_t CS_OFFSET = 9;
  static constexpr uint8_t RS_OFFSET = 10;
  static constexpr uint8_t WR_OFFSET = 11;
  static constexpr uint8_t RD_OFFSET = 12;
  const uint8_t gpio_offset;

  inline void write(uint8_t data) { gpio_put_masked(0xff, data & 0xff); }
  uint8_t read8() { return (gpio_get_all() >> gpio_offset && 0xff); }

  void cs_state(bool state) { gpio_put(CS_OFFSET, state); }

  void reset() {
    gpio_put(RST_OFFSET, false);
    sleep_us(10);
    gpio_put(RST_OFFSET, true);
  }

  void read_strobe(bool state) {
    gpio_put(RD_OFFSET, false);
    sleep_us(1);
    gpio_put(RD_OFFSET, true);
  }

  void data_enable(bool state) { gpio_put(RS_OFFSET, state); }

  void write_strobe() {
    gpio_put(WR_OFFSET, false);
    gpio_put(WR_OFFSET, true);
  }

public:
  /*
   * @brief Constructor, initialises the GPIO with an offset
   * NOTE: It is assumed that data bit of gpio are continuous followed by rst,
   * cs, cd, wr, rd
   */
  parallel_interface(uint8_t gpio_offset) : gpio_offset(gpio_offset) {
    gpio_init_mask(0x1fff << gpio_offset);
    gpio_set_dir_masked(0x1fff << gpio_offset, 0x1fff);
    reset();
    gpio_put(RD_OFFSET, true);
    cs_state(false);
  }

  parallel_interface() = delete;

  void inline write(const uint32_t command, size_t cmd_length,
                    const uint32_t data, size_t data_length) override {
    // cs_state(false);

    do {
      write8cmd((command >> (8 * (cmd_length - 1))) & 0xff);
    } while (--cmd_length);

    do {
      write8data((data >> (8 * (data_length - 1))) & 0xff);
    } while (--data_length);

    // cs_state(true);
  }
  void writeRegister8(uint8_t cmd, uint8_t data) { write(cmd, 1, data, 1); }

  void writeRegister16(uint8_t cmd, uint16_t data) { write(cmd, 1, data, 2); }

  void writeRegister32(uint8_t cmd, uint32_t data) { write(cmd, 1, data, 4); }

  void read(const uint8_t command, uint8_t *data, size_t length) override {}

  void write8data(uint8_t data) {
    data_enable(true);
    write(data);
    write_strobe();
  }

  void write8cmd(uint8_t cmd) {
    data_enable(false);
    write(cmd);
    write_strobe();
    data_enable(true);
  }
};