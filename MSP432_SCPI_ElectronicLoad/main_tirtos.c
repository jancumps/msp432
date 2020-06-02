/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_tirtos.c ========
 */
#include "scpi-def.h"

#include <stdint.h>
#include <stddef.h>
/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <ti/sysbios/BIOS.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Board Header files */
#include "Board.h"

#include "rtos_schedules.h"
#include "scpi-def.h"
#include "eload_api.h"

/* Stack size in bytes */
#define THREADSTACKSIZE    1024

/*
 * The following (weak) function definition is needed in applications
 * that do *not* use the NDK TCP/IP stack:
 */
#if defined(__IAR_SYSTEMS_ICC__)
__weak void NDK_hookInit(int32_t id) {}
#elif defined(__GNUC__) && !defined(__ti__)
void __attribute__((weak)) NDK_hookInit(int32_t id) {}
#else
#pragma WEAK (NDK_hookInit)
void NDK_hookInit(int32_t id) {}
#endif


/**
 * @mainpage electronic load firmware API documentation
 *
 * @section publicapis public accessible APIs
 *
 * All functionality that can be used by user interface, programming interface or other modules are
 * written as API functions.
 * The API knows the current state of the instrument. There's no need to cache any state.
 * The preferred pattern is to ask the instrument for any setting, value or state.
 *
 * @section apilist API list
 *
 * @subsection eloadapi eload core instrument API
 * eload_api.h
 *
 * This API provides the functions to query and alter the core instrument operation.
 * You can get and set values, turn input on and off, change operation mode.
 * It allows low level access to DAC and ADC too. That gives control over virtually
 * everything but is to be used with care.
 *
 * @subsection scpi SCPI API
 * scpi-def.h
 *
 * Available SCPI calls.
 *
 *
 */


/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t           thread;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    /* Call driver init functions */
    Board_initGeneral();

    scpi_instrument_init();

    eloadInit();


    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_UART;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadUART, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_HEARTBEAT;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadHeartBeat, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_DISPLAY;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREAD_STACK_DISPLAY);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadDisplay, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_ADC;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadADC, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_DAC;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadDAC, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_INPUTENABLE;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadInputEnable, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    priParam.sched_priority = THREAD_PRIORITY_TEMPERATUREOVERPROTECTION;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadTemperatureOverProtection, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    //the threadDAC task *must* be registered before this one, else the controller goes into exception
    // because there's no queue declared for DAC messages.
    // todo: reactivate, it seems to have a dpendency with the ADC task that's not funny
    priParam.sched_priority = THREAD_PRIORITY_CONTROL;
    pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }
    retc = pthread_create(&thread, &attrs, threadControl, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    // initiate shared hardware modules only. The ones dedicated to a task are initiated in that task
    GPIO_init();
    I2C_init();


    BIOS_start();

    return (0);
}
