#include "plic.h"
#include "types.h"
#include "param.h"
#include "riscv.h"

/**
 * Initialize the PLIC (Platform-Level Interrupt Controller) by enabling
 * the UART0 interrupt request (IRQ).
 */
void plicinit(void)
{
    *(uint32_t *)(PLIC + UART0_IRQ * 4) = 1; // Enable UART0 IRQ
}

/**
 * plicinithart - Initialize the PLIC (Platform-Level Interrupt Controller) for the current hart (hardware thread).
 * Enables UART0 interrupt and sets priority threshold to 0 for this hart.
 */
void plicinithart(void)
{
    int hart = cpuid();

    // Enable UART0 IRQ for this hart
    *(uint32_t*)PLIC_SENABLE(hart) = (1 << UART0_IRQ);     
    
    // Set this hart's priority threshold to 0.
    *(uint32_t*)PLIC_SPRIORITY(hart) = 0;
}

/**
 * plicclaim - Claims the highest priority pending interrupt for the current hart.
 *
 * @return The interrupt number of the claimed IRQ, or 0 if no interrupt is pending.
 */
int plicclaim(void)
{
    int hart = cpuid();
    int irq = *(uint32_t *)PLIC_SCLAIM(hart); // Claim the highest priority IRQ for this hart

    return irq; // Return the claimed IRQ number
}

/**
 * pliccomplete - Complete handling of a PLIC interrupt request for the current hart.
 * @irq: The interrupt request number to complete.
 *
 * Writes the IRQ number to the PLIC claim register to signal completion.
 */
void pliccomplete(int irq)
{
    int hart = cpuid();

    // Complete the IRQ handling for this hart
    *(uint32_t*)PLIC_SCLAIM(hart) = irq;
}