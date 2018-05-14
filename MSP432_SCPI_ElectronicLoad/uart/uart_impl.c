/*
 * uart.c
 *
 *  Created on: 22 dec. 2016
 *      Author: jancumps
 */


#include <stdint.h>
#include <stdint.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <semaphore.h>

/* Example/Board Header files */
#include "Board.h"

#include "rtos_schedules.h"

#include <uart_impl.h>

#include "scpi/scpi.h"
#include"scpi-def.h"


UART_Handle uart;
sem_t SEM_uart_rx; // this binary semaphore handles uart receiving interrupts

void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num);



/*
 *  ======== mainThread ========
 */
void *threadUART(void *arg0)
{
    char        input;
    UART_Params uartParams;
    int iError;

    /* Call driver init functions */
//    GPIO_init();
    UART_init();

    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;
    uartParams.readMode = UART_MODE_CALLBACK; // the uart uses a read interrupt
    uartParams.readCallback = &UART00_IRQHandler; // function called when the uart interrupt fires

#ifdef SCPI_UART_0
    uart = UART_open(Board_UART0, &uartParams);
#else
#ifdef SCPI_UART_1
    uart = UART_open(Board_UART1, &uartParams);
#else
#error "define a valid UART"
#endif
#endif

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    iError = sem_init(&SEM_uart_rx, 0, 0);
    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);


    /* Loop forever echoing */
    while (1) {
        UART_read(uart, &input, 1);
        iError = sem_wait(&SEM_uart_rx); // when a character is received via UART, the interrupt handler will release the binary semaphore
        while (iError) {
            // POSIX reported error with semaphore. Can't recover.
        }
        // in my case: I get an interrupt for a single character, no need to loop.
        GPIO_toggle(Board_GPIO_LED1); // LED B - visual clue that we've received a request over USB
        scpi_instrument_input((const char *)&input, 1);
        GPIO_toggle(Board_GPIO_LED1); // LED B - visual clue off
    }
}

/*
 * The SCPI lib calls this function to write data back over the SCI2 interface
 * visual clue: dim user LED B after delivering the reply
 */
size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;
    GPIO_toggle(Board_GPIO_LED1); // LED B - visual clue that we send a reply over USB
    UART_write(uart, data, len);
    GPIO_toggle(Board_GPIO_LED1); // LED B - visual clue offB
    return len;
}


/* EUSCI A0 UART ISR - The interrupt handler fr UART0 (USB)
 * this method releases the TI-RTOS binary semaphore SEM_uart_rx
 * and uartFxn() will process the received c
 */

void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num)
{
    sem_post(&SEM_uart_rx);
}

