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

  parallelInterface parallel_interface_inst(0);
  ElegooTFT tft(&parallel_interface_inst);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(CYAN);
  tft.drawRoundRect(0, 0, 20, 20, 20, RED);
  tft.drawRoundRect(300, 0, 20, 20, 20, RED);
  tft.drawRoundRect(0, 220, 20, 20, 20, RED);
  tft.drawRoundRect(300, 220, 20, 20, 20, RED);

  while (1) {

    tft.drawCircle(240, 200, 10, GREEN);
    tft.fillCircle(40, 40, 9, WHITE);
    tft.setTextColor(BLUE);
    tft.setTextSize(4);
    tft.setCursor(0, 52);
    tft.println("Kayas Ahmed");
    tft.setTextColor(MAGENTA);

    tft.setTextSize(2);
    tft.println("123456");
    tft.setTextColor(YELLOW);
    tft.print('\n');
    tft.setTextSize(4);
    tft.print(__DATE__);

    sleep_ms(1000);
  }
}