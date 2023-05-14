#include <stdio.h>

#include "drivers/elegoo_tft.h"
#include "interfaces/parallel_interface.h"
#include "libs/colors.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  parallel_interface parallel_interface_inst(0);
  elegoo_tft elegoo_tft_inst(&parallel_interface_inst);
  elegoo_tft_inst.init();
  elegoo_tft_inst.set_rotation(3);
  elegoo_tft_inst.fillScreen(WHITE);

  while (1) {
    elegoo_tft_inst.drawCircle(240, 200, 10, GREEN);
    elegoo_tft_inst.fillCircle(40, 40, 9, WHITE);
    elegoo_tft_inst.setTextColor(BLUE);
    elegoo_tft_inst.setTextSize(4);
    elegoo_tft_inst.setCursor(50, 50);
    elegoo_tft_inst.println("kayas");
    elegoo_tft_inst.drawRoundRect(180, 50, 20, 20, 20, RED);
    sleep_ms(100);
  }
}