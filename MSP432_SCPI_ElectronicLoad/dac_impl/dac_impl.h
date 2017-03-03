/*
 * dac_impl.h
 *
 *  Created on: 2 mrt. 2017
 *      Author: jancu
 */

#ifndef DAC_IMPL_DAC_IMPL_H_
#define DAC_IMPL_DAC_IMPL_H_

#include <stdint.h>

// for TI-RTOS message size, count the bytes. Currently: 3
typedef struct MsgDAC {
    uint8_t module;
    uint16_t value;
} MsgDAC;

#endif /* DAC_IMPL_DAC_IMPL_H_ */
