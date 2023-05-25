#include "drivers/tft.h"
#include "hardware/irq.h"
#include "hardware/rtc.h"
#include "hardware/timer.h"
#include "interfaces/parallel_interface.h"
#include "libs/colors.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include <math.h>
#include <stdio.h>

parallelInterface parallel_interface_inst(0);
TFT tft(&parallel_interface_inst);

#define CLOCK_CENTER_X 150
#define CLOCK_CENTER_Y 100
#define CLOCK_RADIUS 80
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1,
      hy = 0; // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120,
         ohy = 120; // Saved H, M, S x & y coords

boolean initial = 1;

static int8_t conv2d(const char *p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

static void draw_clock_frame_work() {

  tft.setRotation(3);
  uint16_t x0 = 0, x1 = 0, y0 = 0, y1 = 0;
  tft.fillScreen(LIGHTGREY);

  tft.setTextColor(WHITE, LIGHTGREY); // Adding a background colour erases
                                      // previous text automatically
  // Draw clock face
  tft.fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS - 2, BLACK);
  tft.fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS - 8, WHITE);

  // Draw 12 lines
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * (CLOCK_RADIUS - 2) + CLOCK_CENTER_X;
    y0 = sy * (CLOCK_RADIUS - 2) + CLOCK_CENTER_Y;
    x1 = sx * (CLOCK_RADIUS - 15) + CLOCK_CENTER_X;
    y1 = sy * (CLOCK_RADIUS - 15) + CLOCK_CENTER_Y;

    tft.drawLine(x0, y0, x1, y1, BLACK);
  }

  // Draw 60 dots
  for (int i = 0; i < 360; i += 6) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * (CLOCK_RADIUS - 12) + CLOCK_CENTER_X;
    y0 = sy * (CLOCK_RADIUS - 12) + CLOCK_CENTER_Y;
    // Draw minute markers
    tft.drawPixel(x0, y0, BLACK);

    // Draw main quadrant dots
    if (i == 0 || i == 180)
      tft.fillCircle(x0, y0, 2, BLACK);
    if (i == 90 || i == 270)
      tft.fillCircle(x0, y0, 2, BLACK);
  }

  // tft.fillCircle(120, 121, 3, WHITE);
}

static bool timer_irq(repeating_timer_t *rt) {
  datetime_t t;

  rtc_get_datetime(&t);

  // Pre-compute hand degrees, x & y coords for a fast screen update
  sdeg = t.sec * 6;                      // 0-59 -> 0-354
  mdeg = t.min * 6 + sdeg * 0.01666667;  // 0-59 -> 0-360 - includes seconds
  hdeg = t.hour * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and
  hx = cos((hdeg - 90) * 0.0174532925);
  hy = sin((hdeg - 90) * 0.0174532925);
  mx = cos((mdeg - 90) * 0.0174532925);
  my = sin((mdeg - 90) * 0.0174532925);
  sx = cos((sdeg - 90) * 0.0174532925);
  sy = sin((sdeg - 90) * 0.0174532925);

  if (t.sec == 0 || initial) {
    initial = 0;
    // Erase hour and minute hand positions every minute
    tft.drawLine(ohx, ohy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, WHITE);
    ohx = hx * (CLOCK_RADIUS - 35) + CLOCK_CENTER_X + 1;
    ohy = hy * (CLOCK_RADIUS - 35) + CLOCK_CENTER_Y + 1;
    tft.drawLine(omx, omy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, WHITE);
    omx = mx * (CLOCK_RADIUS - 30) + CLOCK_CENTER_X;
    omy = my * (CLOCK_RADIUS - 30) + CLOCK_CENTER_Y + 1;
  }

  // Redraw new hand positions, hour and minute hands not erased here to
  // avoid flicker
  tft.drawLine(osx, osy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, WHITE);
  osx = sx * (CLOCK_RADIUS - 25) + CLOCK_CENTER_X + 1;
  osy = sy * (CLOCK_RADIUS - 25) + CLOCK_CENTER_Y + 1;
  tft.drawLine(osx, osy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, RED);
  tft.drawLine(ohx, ohy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, BLACK);
  tft.drawLine(omx, omy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, BLACK);
  tft.drawLine(osx, osy, CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, RED);

  tft.fillCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y + 1, 3, RED);
  return true;
}

// Second core main
void main_core2() {
  // Set date and time from compiler time
  // TODO: Get clock from wifi (NTP)
  datetime_t t = {
      .year = 2023,
      .month = 05,
      .day = 20,
      .dotw = 6,  // 0 is Sunday, so 5 is Friday
      .hour = 20, // conv2d(__TIME__),
      .min = 44,  // conv2d(__TIME__ + 3),
      .sec = 0,
  }; // conv2d(__TIME__ + 6)};

  // Start the RTC
  rtc_init();
  rtc_set_datetime(&t);

  // Initialise tft
  tft.init();

  // Make other core aware that TFT init is finish
  multicore_fifo_push_blocking(1);

  // Draw clock frame
  draw_clock_frame_work();

  // Initialise timer with an interrupt every second
  repeating_timer_t timer;
  add_repeating_timer_us(1000000, timer_irq, NULL, &timer);

  // MAIN LOOP for core 2
  while (1) {
    tight_loop_contents();
  }
}

int main() {
  stdio_init_all();

  // start core 2
  multicore_launch_core1(main_core2);

  // Wait for core 2 to finish TFT init
  uint32_t g = multicore_fifo_pop_blocking();

  // MAIN LOOP for core 1
  while (1) {
    tight_loop_contents();
  }
}
