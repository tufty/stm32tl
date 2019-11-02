#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>

// Should move this stuff to the board header
#define HSE_FREQ 8000000

typedef HSE_OSC_T<HSE_FREQ> HSE;
typedef PLL_T<HSE, 72000000> PLL;
typedef SYSCLK_T<PLL> SYSCLK;
typedef SYSTICK_T<SYSCLK> SYSTICK;
typedef TIMEOUT_T<SYSTICK> TIMEOUT;


extern "C" {

  void SysTick_Handler(void) {
    if (TIMEOUT::count_down()) exit_idle();
  }
  
}

// Into our application

typedef GPIO_T<PB, 1, OUTPUT_10MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_PORT_T<PB, LED1> PORT_B;

int main(void)
{
  // Boilerplate, move to board initialisation
  HSE::init();
  PLL::init();
  SYSCLK::init();
  SYSTICK::init();
  RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;

  // App
  PORT_B::init();

  while (1) {
    LED1::set_high();
    SYSTICK::set_and_wait(100);
    LED1::set_low();
    TIMEOUT::set_and_wait(900);
  }
  return 0;
}
