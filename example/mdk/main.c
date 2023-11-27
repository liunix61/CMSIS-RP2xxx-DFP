
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "cmsis_compiler.h"

#include "RTE_Components.h"
#include <assert.h>

#if defined(__PERF_COUNTER__) && __PERF_COUNTER__
#   include "perf_counter.h"
#else
#   define __cycleof__(...)
#endif

#if defined(RTE_Compiler_EventRecorder) && defined(RTE_Compiler_IO_STDOUT_EVR)
#   include <EventRecorder.h>
#   include "EventRecorderConf.h"
#endif

#if EVENT_TIMESTAMP_SOURCE != 1
void SysTick_Handler(void)
{
}
#endif

static void system_init(void)
{
    void SystemCoreClockUpdate (void);
    
    SystemCoreClockUpdate();

#if defined(__PERF_COUNTER__) && __PERF_COUNTER__
    init_cycle_counter(false);
#endif

#if defined(RTE_Compiler_EventRecorder) && defined(RTE_Compiler_IO_STDOUT_EVR)
    EventRecorderInitialize(0, 1);
#endif


    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
}

#define TOP         (0x1FFF)

#ifndef ABS
#   define ABS(__N)    ((__N) < 0 ? -(__N) : (__N))
#endif
#ifndef _BV
#   define _BV(__N)    ((uint32_t)1<<(__N))
#endif


/*! \brief set the 16-level led gradation
 *! \param hwLevel gradation
 *! \return none
 */
static
void set_led_gradation(uint16_t hwLevel)
{
    static uint16_t s_hwCounter = 0;
    
    if (hwLevel >= s_hwCounter) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
    } else {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }
    
    s_hwCounter++;
    s_hwCounter &= TOP;
}

void breath_led(void)
{
    static uint16_t s_hwCounter = 0;
    static int16_t s_nGray = (TOP >> 1);
    
    s_hwCounter++;
    if (!(s_hwCounter & (_BV(10)-1))) {
        s_nGray++; 
        if (s_nGray == TOP) {
            s_nGray = 0;
        }
    }
    
    set_led_gradation(ABS(s_nGray - (TOP >> 1)));
}



int main(void)
{
    system_init();

    __cycleof__() {
        printf("Hello Pico-Template\r\n");
    }

#ifdef __PERF_COUNTER_COREMARK__
    coremark_main();
#endif

    while(1) {
        breath_led();
    }

}