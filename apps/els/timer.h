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

  // Accessor function template for timer registers
  typedef enum { CR1 = 0, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER, CNT, PSC, ARR, RCR, CCR1, CCR2, CCR3, CCR4, BDTR, DCR, DMAR, OR } TIMER_REG;
template <const uint8_t TIMER_NUMBER, TIMER_REG reg> static volatile uint32_t * timer_reg(void) {
    // use of volatile as we may also access registers via bit-banding
    return (volatile uint32_t *)timer_configs[TIMER_NUMBER].base + (reg << 2); 
  }

template <const uint8_t TIMER_NUMBER, const uint8_t CHANNEL_NUMBER>
struct TIMER_CHANNEL_T {
  static constexpr TIMER_REG CCMR = CHANNEL_NUMBER < 3 ? CCMR1 : CCMR2;
  static constexpr TIMER_REG CCR = CHANNEL_NUMBER == 1 ? CCR1 : CHANNEL_NUMBER == 2 ? CCR2 : CHANNEL_NUMBER == 3 ? CCR3 : CCR4;
  
  static constexpr uintptr_t oe_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CCER) + (((CHANNEL_NUMBER - 1) * 4) << 2);
  static void output_enable() {
    *(uint32_t*)oe_bit = 1;
  }
  static void output_disable() {
    *(uint32_t*)oe_bit = 0;
  }

  static constexpr uintptr_t op_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CCER) + ((((CHANNEL_NUMBER - 1) * 4) + 1) << 2);
  typedef enum { OP_HIGH = 0, OP_LOW } OUTPUT_POLARITY;
  static void set_output_polarity(const OUTPUT_POLARITY p) {
    *(uint32_t*)op_bit = p;
  }
  
  static constexpr uintptr_t one_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CCER) + ((((CHANNEL_NUMBER - 1) * 4) + 2) << 2);
  static void output_n_enable() {
    static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Complementary outputs only available on advanced timers");
    *(uint32_t*)one_bit = 1;
  }
  static void output_n_disable() {
    static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Complementary outputs only available on advanced timers");
    *(uint32_t*)one_bit = 0;
  }

  static constexpr uintptr_t onp_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CCER) + ((((CHANNEL_NUMBER - 1) * 4) + 3) << 2);
  static void set_output_n_polarity(const OUTPUT_POLARITY p) {
    static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Complementary outputs only available on advanced timers");
    *(uint32_t*)onp_bit = p;
  }

  static constexpr uintptr_t ois_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CR2) + ((((CHANNEL_NUMBER - 1) * 2) << 8) << 2);
  static void set_output_idle_state(const OUTPUT_POLARITY p) {
    static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Output idle states only available on advanced timers");
    *(uint32_t*)ois_bit = p;
  }

  static constexpr uintptr_t onis_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CR2) + (((((CHANNEL_NUMBER - 1) * 2) << 8) + 1) << 2);
  static void set_output_n_idle_state(const OUTPUT_POLARITY p) {
    static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Output idle states only available on advanced timers");
    *(uint32_t*)onis_bit = p;
  }

  // Brainfarting here, should use the bitbanded stuff
  static void output_preload_enable() {
    constexpr uint8_t shift = 3 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() |= 1 << shift;
  }
  static void output_preload_disable() {
    constexpr uint8_t shift = 3 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() &= ~(1 << shift);
  }
  static void output_fast_enable() {
    constexpr uint8_t shift = 2 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() |= 1 << shift;
  }
  static void output_fast_disable() {
    constexpr uint8_t shift = 2 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() &= ~(1 << shift);
  }
  static void output_clear_enable() {
    constexpr uint8_t shift = 7 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() |= 1 << shift;
  }
  static void output_clear_disable() {
    constexpr uint8_t shift = 7 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    *timer_reg<TIMER_NUMBER, CCMR>() &= ~(1 << shift);
  }
  
  typedef enum { OUTPUT = 0, INPUT_TI2, INPUT_TI1, INPUT_TRC } CAPTURE_COMPARE_STATE;
  static void set_capture_compare_state(const CAPTURE_COMPARE_STATE state) {
    constexpr uint8_t shift = (8 * ((CHANNEL_NUMBER - 1) & 1));
    uint16_t val = *timer_reg<TIMER_NUMBER, CCMR>();
    val &= 0x03 << shift;
    val |= state << shift;
    *timer_reg<TIMER_NUMBER, CCMR>() = val;
  }

  
  typedef enum { FROZEN = 0, ACTIVE_MATCH, INACTIVE_MATCH, FORCE_LOW, FORCE_HIGH, PWM_1, PWM_2 } OC_MODE;
  static void set_oc_mode (const OC_MODE mode) {
    constexpr uint8_t shift = 4 + (8 * ((CHANNEL_NUMBER - 1) & 1));
    
    uint16_t val = *timer_reg<TIMER_NUMBER, CCMR>();
    val &= 0x07 << shift;
    val |= mode << shift;
    *timer_reg<TIMER_NUMBER, CCMR>() = val;
  }

  // Capture-compare register access
  static void set_ccr(const uint16_t val) {
    *timer_reg<TIMER_NUMBER, CCR>() = val;
  }
  static uint16_t ccr() {
    return *timer_reg<TIMER_NUMBER, CCR>();
  }


  
  // DMA and interrupt enable
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

// Base timer functionality
template <const uint8_t TIMER_NUMBER>
struct TIMER_T {
  static_assert(TIMER_NUMBER > 0 && TIMER_NUMBER < 18, "Timers must be in range 1->17");
  static_assert(timer_configs[TIMER_NUMBER].base != 0xdeadbeef, "Timer not defined for this platform");
  
  static void init() {
  }

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

  // Time base setup
  typedef enum { DIVIDE_1 = 0, DIVIDE_2 = 1, DIVIDE_4 = 2 } CLOCK_DIVIDER;
  static void set_divider(const CLOCK_DIVIDER division) {
    uint32_t val = *timer_reg<TIMER_NUMBER, CR1>();
    val &= ~TIM_CR1_CKD_Msk;
    val |= division << TIM_CR1_CKD_Pos;
    *timer_reg<TIMER_NUMBER, CR1>() = val;
  }

  typedef enum { UP_COUNTER = 0, DOWN_COUNTER = 1 } DIRECTION;
  static constexpr uintptr_t direction_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CR1) + BB_BIT(TIM_CR1_DIR_Pos);
  static void set_direction(const DIRECTION direction) {
    *(uint32_t*)direction_bit = direction;
  }

  typedef enum { EDGE_ALIGNED = 0, CENTRE_ALIGNED_1 = 1, CENTRE_ALIGNED_2 = 2, CENTRE_ALIGNED_3 = 3 } ALIGNMENT_MODE;
  static void set_alignment_mode(const ALIGNMENT_MODE alignent_mode) {
    uint32_t val = *timer_reg<TIMER_NUMBER, CR1>();
    val &=  ~TIM_CR1_CMS_Msk;
    val |= alignent_mode << TIM_CR1_CMS_Pos;
    *timer_reg<TIMER_NUMBER, CR1>() = val;
  }

  static void set_counter(uint16_t counter) {
    *timer_reg<TIMER_NUMBER, CNT>() = counter;
  }

  static uint16_t counter() {
    return *timer_reg<TIMER_NUMBER, CNT>();
  }

  static void set_prescaler(uint16_t prescaler) {
    *timer_reg<TIMER_NUMBER, PSC>() = prescaler;
  }

  static void set_reload(uint16_t reload) {
    *timer_reg<TIMER_NUMBER, ARR>() = reload;
  }

  // One pulse mode
  static constexpr uintptr_t opm_bit = timer_configs[TIMER_NUMBER].bb_base + BB_OFFSETOF(TIM_TypeDef, CR1) + BB_BIT(TIM_CR1_OPM_Pos);
  static void one_pulse_mode_enable() {
    *(uint32_t*)opm_bit = 1;
  }
  static void one_pulse_mode_disable() {
    *(uint32_t*)opm_bit = 0;
  }
    
  
  
};

template <const uint8_t TIMER_NUMBER>
struct GP_TIMER_T  : public TIMER_T<TIMER_NUMBER>
{
  static_assert(TIMER_NUMBER > 1 && TIMER_NUMBER < 6, "General Purpose Timers must be in range 2->5");
  // Add channels
  typedef TIMER_CHANNEL_T<TIMER_NUMBER, 1> CH1;
  typedef TIMER_CHANNEL_T<TIMER_NUMBER, 2> CH2;
  typedef TIMER_CHANNEL_T<TIMER_NUMBER, 3> CH3;
  typedef TIMER_CHANNEL_T<TIMER_NUMBER, 4> CH4;

  static void init() {
    TIMER_T<TIMER_NUMBER>::init();
  };


  
  // Generate accessors for all the normal registers
  // TIMER_REG(CR1);
  // TIMER_REG(CR2);
  // TIMER_REG(SMCR);
  // TIMER_REG(DIER);
  // TIMER_REG(SR);
  // TIMER_REG(EGR);
  // TIMER_REG(CCMR1);
  // TIMER_REG(CCMR2);
  // TIMER_REG(CCER);
  // TIMER_REG(CNT);
  // TIMER_REG(PSC);
  // TIMER_REG(ARR);
  // TIMER_REG(RCR);
  // TIMER_REG(CCR1);
  // TIMER_REG(CCR2);
  // TIMER_REG(CCR3);
  // TIMER_REG(CCR4);
  // TIMER_REG(BDTR);
  // TIMER_REG(DCR);
  // TIMER_REG(DMAR);
  // TIMER_REG(OR);

  // Now some specialised stuff using bit banding to access individual bits
  

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

template<const uint8_t TIMER_NUMBER>
struct ADV_TIMER_T : public TIMER_T<TIMER_NUMBER>
{
  static_assert(TIMER_NUMBER == 1 || TIMER_NUMBER == 8, "Only TIM1 and TIM8 are advanced timers");

  static void init() {
    TIMER_T<TIMER_NUMBER>::init();
  }

  static void set_repetition_counter(const uint8_t value) {
    *timer_reg<TIMER_NUMBER, RCR>() = value;
  }
};
