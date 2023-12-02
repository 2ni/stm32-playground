#include "hal.h"

uint32_t SystemCoreClock;  // needed by cmsis
static volatile uint32_t s_ticks;
// static volatile uint64_t s_ticks;

void SysTick_Handler(void) {
  s_ticks++;
}

void delay_ms(uint32_t ms) {
  uint32_t start = s_ticks;
  while ((s_ticks-start) < ms);
}

void _init(void) {
}

void SystemInit(void) {
  clock_init();
}

int main(void) {
  DL("Hello");

  gpio_output(LED_PIN);

  while (1) {
    DL("toggle");
    gpio_toggle(LED_PIN);
    delay_ms(200);
  }
}
