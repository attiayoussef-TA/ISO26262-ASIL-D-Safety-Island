#include "safety_defs.h"

// --- Global Safety State ---
// Function Pointer initialized to the default "Halt and Log" handler.
// Resides in SRAM for O(1) execution access.
volatile SafeStateFunc CriticalFailureHandler = &System_Halt_And_Log;

// --- Safety Routine (The "Panic Button") ---
void System_Halt_And_Log(void) {
    // 1. Context Preservation
    // Write the specific crash signature to Backup SRAM (survives reset)
    *(volatile uint32_t *)(BKPSRAM_BASE) = FAULT_CODE_WATCHDOG;
    
    // 2. System Lock
    while(1) {
        __asm("NOP"); // Prevent optimization of empty loop
    }
}

// --- The Interrupt Handler (The Trigger) ---
void WWDG_IRQHandler(void) {
    // --- STEP 1: ATOMIC ACTUATION ---
    // We use Bit-Banding to set GPIOA Pin 5 (Brakes) to HIGH.
    // This generates a single atomic bus transaction, impossible to interrupt.
    volatile uint32_t *Brake_Atom_Ptr = BITBAND_PERIPH(&(GPIOA->ODR), 5);
    *Brake_Atom_Ptr = 1; 

    // --- STEP 2: TORQUE CUT ---
    // Direct write to Timer Capture/Compare register to kill PWM to engine.
    // Using the raw pointer defined in header for zero-latency access.
    *TIM1_CCR1_RAW = 0x0000;

    // --- STEP 3: EXECUTE RECOVERY STRATEGY ---
    // Jump to the pre-assigned safety handler (Jump Table approach)
    if(CriticalFailureHandler != 0) {
        CriticalFailureHandler();
        
    }
}
