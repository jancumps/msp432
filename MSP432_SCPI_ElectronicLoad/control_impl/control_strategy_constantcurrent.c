/*
 * control_strategy_constantcurrent.c
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#include "control_strategy_constantcurrent.h"
#include "eload_api.h"
#include <stddef.h>

uint32_t ConstantCurrentStrategyControlFunction (uint32_t uTarget) {
    // todo: implement control mechanism - if needed for constant current
    return uTarget;
}

eload_mode ConstantCurrentStrategyGetMode () {
    return ELOAD_MODE_CURRENT;
}

int8_t ConstantCurrentStrategyGetChar() {
    return 'I';
}

void ConstantCurrentSetCurrent(uint32_t uValue) {
    // todo: set the target current via DAC.
    // todo : convert volt/DAC settings
    // todo: float?
    eLoadRawSetDac(0, uValue);
}

void setConstantCurrentStrategy() {
    __setControlStrategy(ConstantCurrentStrategyControlFunction,
                         ConstantCurrentStrategyGetMode,
                         ConstantCurrentStrategyGetChar,
                         ConstantCurrentSetCurrent,
                         NULL);
}


