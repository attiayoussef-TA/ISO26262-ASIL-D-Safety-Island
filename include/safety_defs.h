#ifndef SAFETY_DEFS_H
#define SAFETY_DEFS_H

#include <stdint.h>

// --- Memory Map Constants (STM32 Cortex-M4) ---
#define PERIPH_BASE         0x40000000UL
#define PERIPH_BB_BASE      0x42000000UL // Cortex-M Bit-Band Alias Region
#define BKPSRAM_BASE        0x40024000UL // Battery Backed SRAM (Context Preservation)

// --- Register Definitions (Direct Memory Access) ---
typedef struct { volatile uint32_t ODR; } GPIO_TypeDef;
#define GPIOA_BASE          (0x40020000UL)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)

// --- Critical Actuator Addresses ---
// 0x40010034 = TIM1 Capture/Compare Register 1 (Direct PWM Control)
#define TIM1_CCR1_RAW       ((volatile uint32_t *) 0x40010034UL)

// --- Functional Safety Macros ---
/* * @brief  Converts a Register Address + Bit Number into a unique 32-bit Word Address
 * @note   This prevents Read-Modify-Write race conditions in the pipeline.
 */
#define BITBAND_PERIPH(address, bit) \
    ((volatile uint32_t *)(PERIPH_BB_BASE + \
    (((uint32_t)address) - PERIPH_BASE) * 32 + \
    (bit) * 4))

// --- Safety Types ---
typedef void (*SafeStateFunc)(void);

// --- Fault Codes ---
#define FAULT_CODE_WATCHDOG 0xDEADBEEF
#define FAULT_CODE_DMA      0xBADF00D5

#endif // SAFETY_DEFS_H
