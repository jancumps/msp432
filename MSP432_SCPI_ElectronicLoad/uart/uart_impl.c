/*
 * uart.c
 *
 *  Created on: 22 dec. 2016
 *      Author: jancumps
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

#include "gpio.h"

#include <stdint.h>
#include <uart_impl.h>

#include "scpi/scpi.h"
#include"scpi-def.h"


UART_Handle uart;

Semaphore_Handle SEM_uart_rx; // this binary semaphore handles uart receiving interrupts

void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num);

/*
 *  ======== fnTaskUART ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Void fnTaskUART(UArg arg0, UArg arg1)
{
    char input;
    UART_Params uartParams;
    Error_Block eb;
    Semaphore_Params sem_params;

    // initialise the SCPI interpreter
    scpi_instrument_init();



    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uartParams.readMode = UART_MODE_CALLBACK; // the uart uses a read interrupt
    uartParams.readCallback = &UART00_IRQHandler; // function called when the uart interrupt fires
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    Semaphore_Params_init(&sem_params);
    sem_params.mode = Semaphore_Mode_BINARY;

    SEM_uart_rx = Semaphore_create(0, &sem_params, &eb);

    while (1) {
        UART_read(uart, &input, 1); // prime the uart bus to read the first character, non blocking
        Semaphore_pend(SEM_uart_rx, BIOS_WAIT_FOREVER); // when a character is received via UART, the interrupt handler will release the binary semaphore
        // in my case: I get an interrupt for a single character, no need to loop.
        GPIO_toggle(Board_LED1); // LED B - visual clue that we've received a request over USB
        scpi_instrument_input((const char *)&input, 1);
        GPIO_toggle(Board_LED1); // LED B - visual clue off
    }
}

/*
 * The SCPI lib calls this function to write data back over the SCI2 interface
 * visual clue: dim user LED B after delivering the reply
 */
size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    GPIO_toggle(Board_LED1); // LED B - visual clue that we send a reply over USB
    UART_write(uart, data, len); // todo: check if this needs to be writePolling()
    GPIO_toggle(Board_LED1); // LED B - visual clue offB
    return len;
}


/* EUSCI A0 UART ISR - The interrupt handler fr UART0 (USB)
 * this method releases the TI-RTOS binary semaphore SEM_uart_rx
 * and uartFxn() will process the received c
 */

void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num)
{
    // todo: proper TI-RTOS interrupt handling,
    // http://www.ti.com/lit/ug/spruex3q/spruex3q.pdf
    // 3.4 Hardware Interrupts
    Semaphore_post(SEM_uart_rx);
}



