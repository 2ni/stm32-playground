#include "hal.h"
#include "misc.h"

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
  // disable buffer, or printf will only print if a \n is given
  setvbuf(stdout, NULL, _IONBF, 0);

  char uidstr[27];
  get_uid(uidstr);
  printf("**** Hello from %s ****\n\n", uidstr);

  gpio_output(LED_PIN);

  while (1) {
    printf("toggle\n");
    gpio_toggle(LED_PIN);
    delay_ms(500);
  }
}
