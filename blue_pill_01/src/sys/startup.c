#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;


extern void __libc_init_array();

extern struct _reent * _impure_ptr;

void Reset_Handler(void);

void Reset_Handler(void) {
    /* Copy init values from text to data */
    uint32_t *init_values_ptr = &_sidata;
    uint32_t *data_ptr = &_sdata;

    if (init_values_ptr != data_ptr) {
        for (; data_ptr < &_edata;) {
            *data_ptr++ = *init_values_ptr++;
        }
    }

    /* Clear the zero segment */
    for (uint32_t *bss_ptr = &_sbss; bss_ptr < &_ebss;) {
        *bss_ptr++ = 0;
    }

    /* Initialize stdio related data */
    struct _reent impure_data = _REENT_INIT (impure_data);
    _impure_ptr = &impure_data;

    // set stdout buff size to 50 bytes
    setvbuf(stdout, NULL, _IOLBF, 50);

    /* Initialize libc and global objects */
    __libc_init_array();

    /* Branch to main function */
    int ret = main();

    exit(ret);
}

void HardFault_Handler(void) {
    for(;;);
}

void Default_IRQHandler (void) {
    return;
}

/* Set FreeRTOS interrupt handlers */
extern void vPortSVCHandler (void);
extern void xPortPendSVHandler (void);
extern void xPortSysTickHandler (void);

#define SVC_IRQHandler          vPortSVCHandler
#define PendSV_IRQHandler       xPortPendSVHandler
#define SysTick_IRQHandler      xPortSysTickHandler

void __attribute__((weak)) NMI_Handler (void) { HardFault_Handler(); }
void __attribute__((weak)) MemManageException (void) { HardFault_Handler(); }
void __attribute__((weak)) BusFaultException (void) { HardFault_Handler(); }
void __attribute__((weak)) UsageFaultException (void) { HardFault_Handler(); }
// void __attribute__((weak)) SVC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DebugMonitor (void) { HardFault_Handler(); }
// void __attribute__((weak)) PendSV_IRQHandler (void) { Default_IRQHandler(); }
// void __attribute__((weak)) SysTick_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) WWDG_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) PVD_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TAMPER_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) RTC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) FLASH_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) RCC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI0_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI4_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel4_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel5_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel6_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMAChannel7_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) ADC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USB_HP_CAN_TX_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USB_LP_CAN_RX0_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) CAN_RX1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) CAN_SCE_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI9_5_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM1_BRK_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM1_UP_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM1_TRG_COM_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM1_CC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM4_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) I2C1_EV_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) I2C1_ER_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) I2C2_EV_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) I2C2_ER_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) SPI1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) SPI2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USART1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USART2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USART3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) EXTI15_10_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) RTCAlarm_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) USBWakeUp_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM8_BRK_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM8_UP_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM8_TRG_COM_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM8_CC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) ADC3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) FSMC_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) SDIO_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM5_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) SPI3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) UART4_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) UART5_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM6_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) TIM7_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMA2_Channel1_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMA2_Channel2_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMA2_Channel3_IRQHandler (void) { Default_IRQHandler(); }
void __attribute__((weak)) DMA2_Channel4_5_IRQHandler (void) { Default_IRQHandler(); }

/* vector table */
unsigned int *isr_vector[]
__attribute__ ((section(".isr_vector"))) = {
  (unsigned int *) &_estack,
  (unsigned int *) Reset_Handler,
  (unsigned int *) NMI_Handler,
  (unsigned int *) HardFault_Handler,
  (unsigned int *) MemManageException,
  (unsigned int *) BusFaultException,
  (unsigned int *) UsageFaultException,
  (unsigned int *) 0, // Reserved
  (unsigned int *) 0, // Reserved
  (unsigned int *) 0, // Reserved
  (unsigned int *) 0, // Reserved
  (unsigned int *) SVC_IRQHandler,
  (unsigned int *) DebugMonitor,
  (unsigned int *) 0, // Reserved
  (unsigned int *) PendSV_IRQHandler,
  (unsigned int *) SysTick_IRQHandler,
  (unsigned int *) WWDG_IRQHandler,
  (unsigned int *) PVD_IRQHandler,
  (unsigned int *) TAMPER_IRQHandler,
  (unsigned int *) RTC_IRQHandler,
  (unsigned int *) FLASH_IRQHandler,
  (unsigned int *) RCC_IRQHandler,
  (unsigned int *) EXTI0_IRQHandler,
  (unsigned int *) EXTI1_IRQHandler,
  (unsigned int *) EXTI2_IRQHandler,
  (unsigned int *) EXTI3_IRQHandler,
  (unsigned int *) EXTI4_IRQHandler,
  (unsigned int *) DMAChannel1_IRQHandler,
  (unsigned int *) DMAChannel2_IRQHandler,
  (unsigned int *) DMAChannel3_IRQHandler,
  (unsigned int *) DMAChannel4_IRQHandler,
  (unsigned int *) DMAChannel5_IRQHandler,
  (unsigned int *) DMAChannel6_IRQHandler,
  (unsigned int *) DMAChannel7_IRQHandler,
  (unsigned int *) ADC_IRQHandler,
  (unsigned int *) USB_HP_CAN_TX_IRQHandler,
  (unsigned int *) USB_LP_CAN_RX0_IRQHandler,
  (unsigned int *) CAN_RX1_IRQHandler,
  (unsigned int *) CAN_SCE_IRQHandler,
  (unsigned int *) EXTI9_5_IRQHandler,
  (unsigned int *) TIM1_BRK_IRQHandler,
  (unsigned int *) TIM1_UP_IRQHandler,
  (unsigned int *) TIM1_TRG_COM_IRQHandler,
  (unsigned int *) TIM1_CC_IRQHandler,
  (unsigned int *) TIM2_IRQHandler,
  (unsigned int *) TIM3_IRQHandler,
  (unsigned int *) TIM4_IRQHandler,
  (unsigned int *) I2C1_EV_IRQHandler,
  (unsigned int *) I2C1_ER_IRQHandler,
  (unsigned int *) I2C2_EV_IRQHandler,
  (unsigned int *) I2C2_ER_IRQHandler,
  (unsigned int *) SPI1_IRQHandler,
  (unsigned int *) SPI2_IRQHandler,
  (unsigned int *) USART1_IRQHandler,
  (unsigned int *) USART2_IRQHandler,
  (unsigned int *) USART3_IRQHandler,
  (unsigned int *) EXTI15_10_IRQHandler,
  (unsigned int *) RTCAlarm_IRQHandler,
  (unsigned int *) USBWakeUp_IRQHandler,
  (unsigned int *) TIM8_BRK_IRQHandler,
  (unsigned int *) TIM8_UP_IRQHandler,
  (unsigned int *) TIM8_TRG_COM_IRQHandler,
  (unsigned int *) TIM8_CC_IRQHandler,
  (unsigned int *) ADC3_IRQHandler,
  (unsigned int *) FSMC_IRQHandler,
  (unsigned int *) SDIO_IRQHandler,
  (unsigned int *) TIM5_IRQHandler,
  (unsigned int *) SPI3_IRQHandler,
  (unsigned int *) UART4_IRQHandler,
  (unsigned int *) UART5_IRQHandler,
  (unsigned int *) TIM6_IRQHandler,
  (unsigned int *) TIM7_IRQHandler,
  (unsigned int *) DMA2_Channel1_IRQHandler,
  (unsigned int *) DMA2_Channel2_IRQHandler,
  (unsigned int *) DMA2_Channel3_IRQHandler,
  (unsigned int *) DMA2_Channel4_5_IRQHandler
};
