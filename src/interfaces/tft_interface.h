#pragma once

#include <stddef.h>
#include <stdint.h>

class tftInterface {
public:
  tftInterface() = default;
  virtual ~tftInterface() = default;
  virtual void write(const uint32_t command, size_t cmd_length,
                     const uint32_t data, size_t data_length) = 0;
  virtual void read(const uint8_t command, uint8_t *data, size_t length) = 0;
  virtual void write8data(uint8_t) = 0;
  virtual void write8cmd(uint8_t) = 0;
  virtual void writeRegister8(uint8_t cmd, uint8_t data) = 0;
  virtual void writeRegister16(uint8_t cmd, uint16_t data) = 0;
  virtual void writeRegister32(uint8_t cmd, uint32_t data) = 0;
};