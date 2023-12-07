#include <stm32f3xx.h> // define target STM32F334x8 in Makefile
//#include <stm32f334x8.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define BIT(x) (1UL << (x))
// #define SET_BIT(REG, BIT) ((REG) |= (BIT))
// #define SETBITS(R, CLEARMASK, SETMASK) (R) = ((R) & ~(CLEARMASK)) | (SETMASK)
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))
#define PINNO(pin) (pin & 255) // get number from PIN('A', 5)
#define PINBANK(pin) (pin >> 8) // get port from PIN('A', 5), A=0

// PB3 must be connected to SWO

// PA5 (pin 21) or PB13 (pin 34)
#define LED_PIN PIN('A', 5)

#define GPIO(N) ((GPIO_TypeDef *) (AHB2PERIPH_BASE + 0x400 * (N))) // 0x48000000
enum { GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };
enum { GPIO_OTYPE_PUSH_PULL, GPIO_OTYPE_OPEN_DRAIN };
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH = 3 };
enum { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };

static inline void spin(volatile uint32_t count) {
  while (count--) (void) 0;
}

static inline GPIO_TypeDef *gpio_bank(uint16_t pin) {
  return GPIO(PINBANK(pin));
}

static inline void gpio_toggle(uint16_t pin) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  uint32_t mask = BIT(PINNO(pin));
  gpio->BSRR = mask << (gpio->ODR & mask ? 16 : 0);
}

static inline int gpio_read(uint16_t pin) {
  return gpio_bank(pin)->IDR & BIT(PINNO(pin)) ? 1 : 0;
}

static inline void gpio_write(uint16_t pin, bool val) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  gpio->BSRR = BIT(PINNO(pin)) << (val ? 0 : 16);
}

static inline void gpio_init(uint16_t pin, uint8_t mode, uint8_t type, uint8_t speed, uint8_t pull, uint8_t af) {
  GPIO_TypeDef *gpio = gpio_bank(pin);
  uint8_t n = (uint8_t) (PINNO(pin));
  RCC->AHBENR |= BIT(RCC_AHBENR_GPIOAEN_Pos + PINBANK(pin));  // enable gpio clk
  MODIFY_REG(gpio->OTYPER, 1UL << n, ((uint32_t) type) << n);
  MODIFY_REG(gpio->OSPEEDR, 3UL << (n * 2), ((uint32_t) speed) << (n * 2));
  MODIFY_REG(gpio->PUPDR, 3UL << (n * 2), ((uint32_t) pull) << (n * 2));
  MODIFY_REG(gpio->AFR[n >> 3], 15UL << ((n & 7) * 4), ((uint32_t) af) << ((n & 7) * 4));
  MODIFY_REG(gpio->MODER, 3UL << (n * 2), ((uint32_t) mode) << (n * 2));
}

static inline void gpio_input(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_INPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH,
            GPIO_PULL_NONE, 0);
}

static inline void gpio_output(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_HIGH, GPIO_PULL_NONE, 0);
}

static inline bool timer_expired(volatile uint64_t *t, uint64_t prd, uint64_t now) {
  if (now + prd < *t) *t = 0;                    // Time wrapped? Reset timer
  if (*t == 0) *t = now + prd;                   // Firt poll? Set expiration
  if (*t > now) return false;                    // Not expired yet, return
  *t = (now - *t) > prd ? now + prd : *t + prd;  // Next expiration time
  return true;                                   // Expired, return true
}

static inline void clock_init(void) {
  // FLASH->ACR latency, p. 66
  // 000: clk <= 24MHz      -> 0UL
  // 001: 24 < clk <= 48MHz -> FLASH_ACR_LATENCY_0
  // 010: 48 < clk <= 72MHZ -> FLASH_ACR_LATENCY_1
  // MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY_Msk,

  MODIFY_REG(RCC->CR, RCC_CR_HSEON_Msk, RCC_CR_HSEON); // external clock 8MHz crystal
  MODIFY_REG(RCC->CR, RCC_CR_HSION_Msk, 0UL); // turn off internal clock
  while (!(RCC->CR & RCC_CR_HSERDY));

  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW_Msk, RCC_CFGR_SW_1); // select PLL as input
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMUL_Msk, RCC_CFGR_PLLMUL2); // PLLMUL = *2
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC_Msk, RCC_CFGR_PLLSRC_HSE_PREDIV); // System Clock Mux = PLLCLK

  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PREDIV_Msk, RCC_CFGR2_PREDIV_DIV1); // HSE prescaler = /1

  // enable PLL
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY));

  // RTC
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
  SET_BIT(PWR->CR, PWR_CR_DBP); // DBP in PWR_CR must be set to modify LSEON, LSEBYP, RTCSEL, RTCEN

  MODIFY_REG(RCC->BDCR, RCC_BDCR_LSEDRV_Msk, RCC_BDCR_LSEDRV_0); // medium drive
  SET_BIT(RCC->BDCR, RCC_BDCR_LSEON); // enable LSE
  while (!(RCC->BDCR & RCC_BDCR_LSERDY));

  MODIFY_REG(RCC->BDCR, RCC_BDCR_RTCSEL_Msk, RCC_BDCR_RTCSEL_LSE); // LSE is source of RTC
  SET_BIT(RCC->BDCR, RCC_BDCR_RTCEN); // enable RTC

  SystemCoreClock = 16000000;
  MODIFY_REG(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN_Msk, RCC_APB2ENR_SYSCFGEN); // enable clock ticks
  SysTick_Config(SystemCoreClock / 1000);
}
