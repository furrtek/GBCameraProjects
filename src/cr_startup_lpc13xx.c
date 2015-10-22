//*****************************************************************************
// LPC13xx Microcontroller Startup code for use with LPCXpresso IDE
//
// Version : 150706
//*****************************************************************************
//
// Copyright(C) NXP Semiconductors, 2013-2015
// All rights reserved.
//
// Software that is described herein is for illustrative purposes only
// which provides customers with programming information regarding the
// LPC products.  This software is supplied "AS IS" without any warranties of
// any kind, and NXP Semiconductors and its licensor disclaim any and
// all warranties, express or implied, including all implied warranties of
// merchantability, fitness for a particular purpose and non-infringement of
// intellectual property rights.  NXP Semiconductors assumes no responsibility
// or liability for the use of the software, conveys no license or rights under any
// patent, copyright, mask work right, or any other intellectual property rights in
// or to any products. NXP Semiconductors reserves the right to make changes
// in the software without notification. NXP Semiconductors also makes no
// representation or warranty that such application will be suitable for the
// specified use without further testing or modification.
//
// Permission to use, copy, modify, and distribute this software and its
// documentation is hereby granted, under NXP Semiconductors' and its
// licensor's relevant copyrights in the software, without fee, provided that it
// is used in conjunction with NXP Semiconductors microcontrollers.  This
// copyright, permission, and disclaimer notice must appear in all copies of
// this code.
//*****************************************************************************

#include "LPC13xx.h"

#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern "C" {
    extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
#if defined (__cplusplus)
extern "C" {
#endif

//*****************************************************************************
#if defined (__USE_CMSIS) || defined (__USE_LPCOPEN)
// Declaration of external SystemInit function
extern void SystemInit(void);
#endif

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will 
// automatically take precedence over these weak definitions
//
//*****************************************************************************
void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take 
// precedence over these weak definitions
//
//*****************************************************************************
void I2C_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER16_0_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER16_1_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER32_0_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER32_1_IRQHandler(void) ALIAS(IntDefaultHandler);
#if defined (__USE_LPCOPEN)
void SSP0_IRQHandler(void) ALIAS(IntDefaultHandler);
#else
void SSP_IRQHandler(void) ALIAS(IntDefaultHandler);
#endif
void UART_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_FIQHandler(void) ALIAS(IntDefaultHandler);
WEAK void ADC_IRQHandler(void) ALIAS(IntDefaultHandler);
void WDT_IRQHandler(void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler(void) ALIAS(IntDefaultHandler);
void FMC_IRQHandler(void) ALIAS(IntDefaultHandler);
void PIOINT3_IRQHandler(void) ALIAS(IntDefaultHandler);
WEAK void PIOINT2_IRQHandler(void) ALIAS(IntDefaultHandler);
void PIOINT1_IRQHandler(void) ALIAS(IntDefaultHandler);
void PIOINT0_IRQHandler(void) ALIAS(IntDefaultHandler);
void WAKEUP_IRQHandler(void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for Redlib based applications
// main() is the entry point for Newlib based applications
//
//*****************************************************************************
#if defined (__REDLIB__)
extern void __main(void);
#else
extern int main(void);
#endif
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
//
// External declaration for LPC MCU vector table checksum from  Linker Script
//
//*****************************************************************************
WEAK extern void __valid_user_code_checksum();

//*****************************************************************************
#if defined (__cplusplus)
} // extern "C"
#endif
//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    // Core Level - CM3
        &_vStackTop, // The initial stack pointer
        ResetISR, // The reset handler
        NMI_Handler, // The NMI handler
        HardFault_Handler, // The hard fault handler
        MemManage_Handler, // The MPU fault handler
        BusFault_Handler, // The bus fault handler
        UsageFault_Handler, // The usage fault handler
        __valid_user_code_checksum,  // LPC MCU Checksum
        0, // Reserved
        0, // Reserved
        0, // Reserved
        SVC_Handler, // SVCall handler
        DebugMon_Handler, // Debug monitor handler
        0, // Reserved
        PendSV_Handler, // The PendSV handler
        SysTick_Handler, // The SysTick handler


        // Wakeup sources (40 ea.) for the I/O pins:
        //   PIO0 (0:11)
        //   PIO1 (0:11)
        //   PIO2 (0:11)
        //   PIO3 (0:3)
        WAKEUP_IRQHandler, // PIO0_0  Wakeup
        WAKEUP_IRQHandler, // PIO0_1  Wakeup
        WAKEUP_IRQHandler, // PIO0_2  Wakeup
        WAKEUP_IRQHandler, // PIO0_3  Wakeup
        WAKEUP_IRQHandler, // PIO0_4  Wakeup
        WAKEUP_IRQHandler, // PIO0_5  Wakeup
        WAKEUP_IRQHandler, // PIO0_6  Wakeup
        WAKEUP_IRQHandler, // PIO0_7  Wakeup
        WAKEUP_IRQHandler, // PIO0_8  Wakeup
        WAKEUP_IRQHandler, // PIO0_9  Wakeup
        WAKEUP_IRQHandler, // PIO0_10 Wakeup
        WAKEUP_IRQHandler, // PIO0_11 Wakeup

        WAKEUP_IRQHandler, // PIO1_0  Wakeup
        WAKEUP_IRQHandler, // PIO1_1  Wakeup
        WAKEUP_IRQHandler, // PIO1_2  Wakeup
        WAKEUP_IRQHandler, // PIO1_3  Wakeup
        WAKEUP_IRQHandler, // PIO1_4  Wakeup
        WAKEUP_IRQHandler, // PIO1_5  Wakeup
        WAKEUP_IRQHandler, // PIO1_6  Wakeup
        WAKEUP_IRQHandler, // PIO1_7  Wakeup
        WAKEUP_IRQHandler, // PIO1_8  Wakeup
        WAKEUP_IRQHandler, // PIO1_9  Wakeup
        WAKEUP_IRQHandler, // PIO1_10 Wakeup
        WAKEUP_IRQHandler, // PIO1_11 Wakeup

        WAKEUP_IRQHandler, // PIO2_0  Wakeup
        WAKEUP_IRQHandler, // PIO2_1  Wakeup
        WAKEUP_IRQHandler, // PIO2_2  Wakeup
        WAKEUP_IRQHandler, // PIO2_3  Wakeup
        WAKEUP_IRQHandler, // PIO2_4  Wakeup
        WAKEUP_IRQHandler, // PIO2_5  Wakeup
        WAKEUP_IRQHandler, // PIO2_6  Wakeup
        WAKEUP_IRQHandler, // PIO2_7  Wakeup
        WAKEUP_IRQHandler, // PIO2_8  Wakeup
        WAKEUP_IRQHandler, // PIO2_9  Wakeup
        WAKEUP_IRQHandler, // PIO2_10 Wakeup
        WAKEUP_IRQHandler, // PIO2_11 Wakeup

        WAKEUP_IRQHandler, // PIO3_0  Wakeup
        WAKEUP_IRQHandler, // PIO3_1  Wakeup
        WAKEUP_IRQHandler, // PIO3_2  Wakeup
        WAKEUP_IRQHandler, // PIO3_3  Wakeup

        I2C_IRQHandler, // I2C0
        TIMER16_0_IRQHandler, // CT16B0 (16-bit Timer 0)
        TIMER16_1_IRQHandler, // CT16B1 (16-bit Timer 1)
        TIMER32_0_IRQHandler, // CT32B0 (32-bit Timer 0)
        TIMER32_1_IRQHandler, // CT32B1 (32-bit Timer 1)
#if defined (__USE_LPCOPEN)
        SSP0_IRQHandler, // SSP0
#else
        SSP_IRQHandler, // SSP0
#endif
        UART_IRQHandler, // UART0

        USB_IRQHandler, // USB IRQ
        USB_FIQHandler, // USB FIQ

        ADC_IRQHandler, // ADC   (A/D Converter)
        WDT_IRQHandler, // WDT   (Watchdog Timer)
        BOD_IRQHandler, // BOD   (Brownout Detect)
        FMC_IRQHandler, // Flash (IP2111 Flash Memory Controller)
        PIOINT3_IRQHandler, // PIO INT3
        PIOINT2_IRQHandler, // PIO INT2
        PIOINT1_IRQHandler, // PIO INT1
        PIOINT0_IRQHandler, // PIO INT0
        SSP1_IRQHandler, // SSP1

    };

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int *pulSrc = (unsigned int*) romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len) {
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}

//*****************************************************************************
// The following symbols are constructs generated by the linker, indicating
// the location of various points in the "Global Section Table". This table is
// created by the linker via the Code Red managed linker script mechanism. It
// contains the load address, execution address and length of each RW data
// section and the execution and length of each BSS (zero initialized) section.
//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void
ResetISR(void) {

    //
    // Copy the data sections from flash to SRAM.
    //
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }
    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }
    
    // Optionally enable Cortex-M3 SWV trace (off by default at reset)
    // Note - your board support must also set up pinmuxing such that
    // SWO is output on GPIO PIO0-9 (FUNC3)
#if !defined (DONT_ENABLE_SWVTRACECLK)
	// Write 0x1 to TRACECLKDIV – Trace divider
	volatile unsigned int *TRACECLKDIV = (unsigned int *) 0x400480AC;
	*TRACECLKDIV = 1;
#endif

#if defined (__USE_CMSIS) || defined (__USE_LPCOPEN)
    SystemInit();
#endif

#if defined (__cplusplus)
    //
    // Call C++ library initialisation
    //
    __libc_init_array();
#endif

#if defined (__REDLIB__)
    // Call the Redlib library, which in turn calls main()
    __main() ;
#else
    main();
#endif
    //
    // main() shouldn't return, but if it does, we'll just enter an infinite loop
    //
    while (1) {
        ;
    }
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void SVC_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void PendSV_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
__attribute__ ((section(".after_vectors")))
void SysTick_Handler(void) {
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}

//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a handler
// is not present in the application code.
//
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void) {
    //
    // Go into an infinite loop.
    //
    while (1) {
    	LPC_GPIO1->DATA ^= (1<<5);	// Debug
    }
}
