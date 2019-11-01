#pragma once

//#include <usb/usb.h>
//#include <drivers/usb_cdc.h>

typedef GPIO_PORT_T<PA> PORT_A;
typedef GPIO_PORT_T<PB> PORT_B;
typedef GPIO_PORT_T<PC> PORT_C;

typedef GPIO_OUTPUT_T<PB, 1> LED1;
typedef GPIO_T<PB, 8, INPUT, FLOATING, LOW, INTERRUPT_ENABLED, EDGE_FALLING> BUTTON;

typedef GPIO_OUTPUT_T<PB, 9> USB_DISC;

typedef GPIO_T<PA, 5, OUTPUT_50MHZ, ALT_PUSH_PULL> SCK;
typedef GPIO_T<PA, 6, INPUT> MISO;
typedef GPIO_T<PA, 7, OUTPUT_50MHZ, ALT_PUSH_PULL> MOSI;

//typedef USB_T::CDC CON;

namespace BOARD_T {

enum {
	LED1 = 1,
	BUTTON = 4,
	CONSOLE = 8,
	I2C = 16,
	SPI = 32
};

  template<const uint32_t FEATURES = LED1 | BUTTON | CONSOLE | I2C | SPI>
  struct BOARD {
    static void init(void) {
      
      PORT_A::init();
      PORT_B::init();
      PORT_C::init();

      
      if (FEATURES & LED1) {
	LED1::init();
      }
	  
      if (FEATURES & SPI) {
	SCK::init();
	MOSI::init();
	MISO::init();
      }
      if (FEATURES & BUTTON) {
	BUTTON::init();
      }
      /*      if (FEATURES & CONSOLE) {
	USB_DISC::init();
	USB_DISC::set_high();
	CON::init();
	USB_DISC::set_low();
	} */
    }
  };

};

extern "C" {

  //void USB_LP_CAN1_RX0_IRQHandler(void) {
  //	if (CON::handle_irq()) exit_idle();
  //}

}
