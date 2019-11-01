#pragma once

// remove typed defines
#undef TIM1
#undef TIM2
#undef TIM3
#undef TIM4
#undef TIM5
#undef TIM6
#undef TIM7
#undef TIM8
#undef TIM9
#undef TIM10
#undef TIM11
#undef TIM12
#undef TIM13
#undef TIM14
#undef TIM15
#undef TIM16
#undef TIM17

static constexpr TIM_TypeDef *timer_addr[18] = {
	0,
	((TIM_TypeDef *)TIM1_BASE),
#ifdef TIM2_BASE
	((TIM_TypeDef *)TIM2_BASE),
#else
	0,
#endif
#ifdef TIM3_BASE
	((TIM_TypeDef *)TIM3_BASE),
#else
	0,
#endif
#ifdef TIM4_BASE
	((TIM_TypeDef *)TIM4_BASE),
#else
	0,
#endif
#ifdef TIM5_BASE
	((TIM_TypeDef *)TIM5_BASE),
#else
	0,
#endif
#ifdef TIM6_BASE
	((TIM_TypeDef *)TIM6_BASE),
#else
	0,
#endif
#ifdef TIM7_BASE
	((TIM_TypeDef *)TIM7_BASE),
#else
	0,
#endif
#ifdef TIM8_BASE
	((TIM_TypeDef *)TIM8_BASE),
#else
	0,
#endif
#ifdef TIM9_BASE
	((TIM_TypeDef *)TIM9_BASE),
#else
	0,
#endif
#ifdef TIM10_BASE
	((TIM_TypeDef *)TIM10_BASE),
#else
	0,
#endif
#ifdef TIM11_BASE
	((TIM_TypeDef *)TIM11_BASE),
#else
	0,
#endif
#ifdef TIM12_BASE
	((TIM_TypeDef *)TIM12_BASE),
#else
	0,
#endif
#ifdef TIM13_BASE
	((TIM_TypeDef *)TIM13_BASE),
#else
	0,
#endif
#ifdef TIM14_BASE
	((TIM_TypeDef *)TIM14_BASE),
#else
	0,
#endif
#ifdef TIM15_BASE
	((TIM_TypeDef *)TIM15_BASE),
#else
	0,
#endif
#ifdef TIM16_BASE
	((TIM_TypeDef *)TIM16_BASE),
#else
	0,
#endif
#ifdef TIM17_BASE
	((TIM_TypeDef *)TIM17_BASE),
#else
	0,
#endif
};

template <const int TIMER_NUMBER,
	 const int CHANNEL_NUMBER,
	 const int FILTER = 0,
	 const int PRESCALER = 0,
	 const int INPUT_SOURCE = 1,
	 const int EDGE = EDGE_RISING>
struct INPUT_TIMER_CHANNEL_T {
	static constexpr TIM_TypeDef *timer = timer_addr[TIMER_NUMBER];
	static constexpr int number = CHANNEL_NUMBER - 1;
	static constexpr bool input = true;
	static constexpr bool output = false;
	static constexpr int filter = FILTER;
	static constexpr int prescaler = PRESCALER;
	static constexpr int edge = EDGE;

	static void enable(void) {
		timer->CCR1 = 0;
		timer->CCER &= ~(0xf << number * 4);
		timer->CCER |= (1 << number * 4) |
			(edge == EDGE_FALLING ? 0x2 << number : 0) |
			(edge == EDGE_BOTH ? 0xa << number : 0);
	}

	static void disable(void) {
		timer->CCER &= ~(1 << number * 4);
	}

	static void set_mode(void) {
		uint32_t mode = 0;

		mode = INPUT_SOURCE | prescaler << 2 | filter << 4;
		switch (number) {
			case 0: timer->CCMR1 = mode; break;
			case 1: timer->CCMR1 = mode << 8; break;
			case 2: timer->CCMR2 = mode; break;
			case 3: timer->CCMR2 = mode << 8; break;
		}
	}

	static void enable_irq(void) {
		timer->DIER |= (2 << number);
		switch (TIMER_NUMBER) {
#ifdef TIM1_BASE
			case 1:
			  NVIC_ClearPendingIRQ(TIM1_BRK_IRQn);
			  NVIC_EnableIRQ(TIM1_BRK_IRQn);
			  NVIC_ClearPendingIRQ(TIM1_UP_IRQn);
			  NVIC_EnableIRQ(TIM1_UP_IRQn);
			  NVIC_ClearPendingIRQ(TIM1_TRG_COM_IRQn);
			  NVIC_EnableIRQ(TIM1_TRG_COM_IRQn);
			  NVIC_ClearPendingIRQ(TIM1_CC_IRQn);
			  NVIC_EnableIRQ(TIM1_CC_IRQn);
			  break;
#endif
#ifdef TIM2_BASE
			case 2:
			  NVIC_ClearPendingIRQ(TIM2_IRQn);
			  NVIC_EnableIRQ(TIM2_IRQn);
			  break;
#endif
#ifdef TIM3_BASE		  
			case 3:
			  NVIC_ClearPendingIRQ(TIM3_IRQn);
			  NVIC_EnableIRQ(TIM3_IRQn);
			  break;
#endif
#ifdef TIM4_BASE
			case 4:
			  NVIC_ClearPendingIRQ(TIM4_IRQn);
			  NVIC_EnableIRQ(TIM4_IRQn);
			  break;
#endif		  
		}
	}

	static void disable_irq(void) {
	}

};

struct UNUSED_CHANNEL {
	static constexpr int number = -1;
};

template <const int TIMER_NUMBER,
	 typename CH1 = UNUSED_CHANNEL,
	 typename CH2 = UNUSED_CHANNEL,
	 typename CH3 = UNUSED_CHANNEL,
	 typename CH4 = UNUSED_CHANNEL>
struct TIMER_T {
	static constexpr TIM_TypeDef *timer = timer_addr[TIMER_NUMBER];

	static void init(void) {
		switch (TIMER_NUMBER) {
			case 1: RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; break;
			case 2: RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; break;
			case 3: RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; break;
			case 4: RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; break;
		}
	}

	static void disable(void) {
		switch (TIMER_NUMBER) {
			case 1: RCC->APB2ENR &= ~RCC_APB2ENR_TIM1EN; break;
			case 2: RCC->APB1ENR &= ~RCC_APB1ENR_TIM2EN; break;
			case 3: RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN; break;
			case 4: RCC->APB1ENR &= ~RCC_APB1ENR_TIM4EN; break;
		}
	}

	static void counter_start(void) {
		timer->CNT = 0;
		timer->CR1 |= TIM_CR1_CEN;
	}

	static void counter_stop(void) {
		timer->CR1 &= ~TIM_CR1_CEN;
	}

	template <typename CHANNEL>
	static void enable_channel(void) {
		timer->CCER &= ~(0xf << CHANNEL::number * 4);
		timer->CCER |= (1 << CHANNEL::number * 4) |
			(CHANNEL::edge == EDGE_FALLING ? 0x2 << CHANNEL::number : 0) |
			(CHANNEL::edge == EDGE_BOTH ? 0xa << CHANNEL::number : 0);
	}
};
