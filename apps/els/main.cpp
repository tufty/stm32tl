#include <clocks.h>
#include <tasks.h>
#include <gpio.h>
#include <stddef.h>
#include <io.h>
#include "timer.h"

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

typedef GPIO_T<PB, 1, OUTPUT_50MHZ, PUSH_PULL, LOW> LED1;
typedef GPIO_PORT_T<PB, LED1> PORT_B;


typedef GP_TIMER_T<3> TIM3;


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
  LED1::init();

  TIM3::init();
  TIM3::enable();

  TIM3::set_direction(TIM3::UP_COUNTER);
  TIM3::set_divider(TIM3::DIVIDE_1);
  TIM3::set_prescaler(40000);
  TIM3::set_reload(1000);

  TIM3::CH4::set_capture_compare_state(TIM3::CH4::OUTPUT);
  TIM3::CH4::output_enable();
  TIM3::CH4::set_oc_mode(TIM3::CH4::PWM_1);
  TIM3::CH4::set_ccr(400);
  TIM3::CH4::set_output_polarity(TIM3::CH4::OP_HIGH);
  TIM3::CH4::output_preload_enable();

  
  
  TIM3::run();
  
  
  TIM3::CH1::irq_enable();
  
  while (1) {
    TIMEOUT::set_and_wait(900);
  }
  return 0;
}
