/*
 * dac_impl.h
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu
 */

#ifndef DAC_IMPL_DAC_IMPL_H_
#define DAC_IMPL_DAC_IMPL_H_

#include <stdint.h>

#define QUEUE_NAME_DAC "/dac_queue"


typedef struct MsgDAC {
    uint8_t module;
    uint16_t value;
} MsgDAC;

// for TI-RTOS message size, use the structure size:
#define MSGDAC_SIZE (sizeof(MsgDAC))

#endif /* DAC_IMPL_DAC_IMPL_H_ */
