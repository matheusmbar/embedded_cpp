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

void nmi_handler(void) {
    for(;;);
}

void hardfault_handler(void) {
    for(;;);
}

/* vector table */
unsigned int *myvectors[4]
__attribute__ ((section(".isr_vector"))) = {
    (unsigned int *) &_estack,
    (unsigned int *) Reset_Handler,
    (unsigned int *) nmi_handler,
    (unsigned int *) hardfault_handler
};
