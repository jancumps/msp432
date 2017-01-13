/*
 * eload_api.c
 *
 *  Created on: 24 dec. 2016
 *      Author: jancu
 */

#include <stdint.h>
#include "eload_api.h"

eload_mode eloadMode = ELOAD_MODE_CURRENT;

double eloadGetVoltageDC() {
    return (double)3.141592653589793; // todo: get input voltage from the sampled value
}


void eloadSetMode(eload_mode mode){
    // todo: stop PID engine and reinitialise strategy
    eloadMode = mode;
}

eload_mode eloadGetMode() {
    return eloadMode;
}

uint32_t eLoadGetCurrentRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eLoadGetVoltageRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eLoadGetOutputRangeMax() {
    return 0b1111111111111111; // 16 bit DAC
}
