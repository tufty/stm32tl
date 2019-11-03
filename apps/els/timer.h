#pragma once

#undef TIM1
#undef TIM2
#undef TIM3
#undef TIM4
#undef TIM5
#undef TIM6
#undef TIM7
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


#define RCC_BB_BASE (PERIPH_BB_BASE) + (((RCC_BASE) - (PERIPH_BASE)) << 5)
#define TIM1_BB_BASE (PERIPH_BB_BASE) + (((TIM1_BASE) - (PERIPH_BASE)) << 5)
#define TIM2_BB_BASE (PERIPH_BB_BASE) + (((TIM2_BASE) - (PERIPH_BASE)) << 5)
#define TIM3_BB_BASE (PERIPH_BB_BASE) + (((TIM3_BASE) - (PERIPH_BASE)) << 5)
#define TIM4_BB_BASE (PERIPH_BB_BASE) + (((TIM4_BASE) - (PERIPH_BASE)) << 5)

#define BB_OFFSETOF(a, b) (offsetof(a, b) << 5)
#define BB_BIT(a) ((a) << 2)

// Varies from timer to timer
static constexpr struct {
  uintptr_t base;
  uintptr_t bb_base;
  uintptr_t rcc_enable_offset;
  
} timer_configs[18] =
  {
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef}, // Empty to avoid having to do math on the timer number
   { TIM1_BASE, TIM1_BB_BASE, BB_OFFSETOF(RCC_TypeDef, APB2ENR) + BB_BIT(RCC_APB2ENR_TIM1EN_Pos) },
   { TIM2_BASE, TIM2_BB_BASE, BB_OFFSETOF(RCC_TypeDef, APB1ENR) + BB_BIT(RCC_APB1ENR_TIM2EN_Pos) },
   { TIM3_BASE, TIM3_BB_BASE, BB_OFFSETOF(RCC_TypeDef, APB1ENR) + BB_BIT(RCC_APB1ENR_TIM3EN_Pos) },
   { TIM4_BASE, TIM4_BB_BASE, BB_OFFSETOF(RCC_TypeDef, APB1ENR) + BB_BIT(RCC_APB1ENR_TIM4EN_Pos) },
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
   { 0xdeadbeef, 0xdeadbeef, 0xdeadbeef},
  };


#define TIMER_REG(x) \
  static constexpr uintptr_t _ ## x = timer_configs[TIMER_NUMBER].base + offsetof(TIM_TypeDef, x);

#define TIMER_ACCESSOR(x)						\
  TIMER_REG(x)								\
  static volatile uint32_t x () {					\
    return *(uint32_t *)_ ## x ;					\
  }									\
  static void x (uint32_t value) {					\
    *(uint32_t *)_ ## x = value;					\
  }


template <const uint8_t TIMER_NUMBER, const uint8_t CHANNEL_NUMBER>
struct TIMER_CHANNEL_T {
  static constexpr uintptr_t dma_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + ((8 + CHANNEL_NUMBER) << 2);
  static constexpr uintptr_t irq_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + (CHANNEL_NUMBER << 2);
  
  static void dma_enable() {
    *(uint32_t*)dma_enable_bit = 1;
  }
  static void dma_disable() {
    *(uint32_t*)dma_enable_bit = 0;
  }
  static void irq_enable() {
    *(uint32_t*)irq_enable_bit = 1;
  }
  static void irq_disable() {
    *(uint32_t*)irq_enable_bit = 0;
  }
};

template <const uint8_t TIMER_NUMBER>
struct TIMER_T {
  static_assert(TIMER_NUMBER > 0 && TIMER_NUMBER < 18, "Timers must be in range 1->17");
  static_assert(timer_configs[TIMER_NUMBER].base != 0xdeadbeef, "Timer not defined for this platform");
};

template <const uint8_t TIMER_NUMBER>
struct GP_TIMER_T  : public TIMER_T<TIMER_NUMBER>
{
  static_assert(TIMER_NUMBER > 1 && TIMER_NUMBER < 6, "General Purpose Timers must be in range 2->5");
  // Generate accessors for all the normal registers
  TIMER_REG(CR1);
  TIMER_REG(CR2);
  TIMER_REG(SMCR);
  TIMER_REG(DIER);
  TIMER_REG(SR);
  TIMER_REG(EGR);
  TIMER_REG(CCMR1);
  TIMER_REG(CCMR2);
  TIMER_REG(CCER);
  TIMER_REG(CNT);
  TIMER_REG(PSC);
  TIMER_REG(ARR);
  TIMER_REG(RCR);
  TIMER_REG(CCR1);
  TIMER_REG(CCR2);
  TIMER_REG(CCR3);
  TIMER_REG(CCR4);
  TIMER_REG(BDTR);
  TIMER_REG(DCR);
  TIMER_REG(DMAR);
  TIMER_REG(OR);

  // Now some specialised stuff using bit banding to access individual bits
  
  // Overall timer enabling in RCC
  static constexpr uintptr_t rcc_enable_bit = RCC_BB_BASE + timer_configs[TIMER_NUMBER].rcc_enable_offset;
  static void enable() {
    *(uint32_t*)(rcc_enable_bit) = 1;
  }

  // Timer run and stop
  static constexpr uintptr_t enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CR1) + BB_BIT(TIM_CR1_CEN_Pos);
  static void run() {
    *(uint32_t*)enable_bit = 1;
  }
  static void stop() {
    *(uint32_t*)enable_bit = 0;
  }

  // Overall DMA and interrupt, see individual channels for channel level stuff
  static constexpr uintptr_t udma_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + BB_BIT(TIM_DIER_UDE_Pos);
  static constexpr uintptr_t tdma_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + BB_BIT(TIM_DIER_TDE_Pos);
  static constexpr uintptr_t uirq_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + BB_BIT(TIM_DIER_UIE_Pos);
  static constexpr uintptr_t tirq_enable_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, DIER) + BB_BIT(TIM_DIER_TIE_Pos);
  static void udma_enable() {
    *(uint32_t*)udma_enable_bit = 1;
  }
  static void udma_disable() {
    *(uint32_t*)udma_enable_bit = 0;
  }
  static void tdma_enable() {
    *(uint32_t*)tdma_enable_bit = 1;
  }
  static void tdma_disable() {
    *(uint32_t*)tdma_enable_bit = 0;
  }
  static void uirq_enable() {
    *(uint32_t*)uirq_enable_bit = 1;
  }
  static void uirq_disable() {
    *(uint32_t*)uirq_enable_bit = 0;
  }
  static void tirq_enable() {
    *(uint32_t*)tirq_enable_bit = 1;
  }
  static void tirq_disable() {
    *(uint32_t*)tirq_enable_bit = 0;
  }


  
  
  
  // Remapping to different IOs 
  static void remap(const uint32_t value) {
    switch (TIMER_NUMBER) {
    case 1:
      AFIO->MAPR &= ~AFIO_MAPR_TIM1_REMAP_Msk;
      AFIO->MAPR |= (value & 0x3) << AFIO_MAPR_TIM1_REMAP_Pos;
      break;
    case 2:
      AFIO->MAPR &= ~AFIO_MAPR_TIM2_REMAP_Msk;
      AFIO->MAPR |= (value & 0x3) << AFIO_MAPR_TIM2_REMAP_Pos;
      break;
    case 3:
      AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_Msk;
      AFIO->MAPR |= (value & 0x3) << AFIO_MAPR_TIM3_REMAP_Pos;
      break;
    case 4:
      AFIO->MAPR &= ~AFIO_MAPR_TIM4_REMAP_Msk;
      AFIO->MAPR |= (value & 0x1) << AFIO_MAPR_TIM4_REMAP_Pos;
      break;
    }
      
  }
  
};
