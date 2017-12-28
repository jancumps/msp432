/*
 * control_strategy_constantcurrent.c
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#include "control_strategy_constantcurrent.h"
#include "eload_api.h"
#include <stddef.h>
#include <stdbool.h>

uint32_t uConstantCurrent;
bool bConstantCurrentChanged;

void ConstantCurrentStrategyControlFunction () {
    if (bConstantCurrentChanged) {
        eloadRawSetDac(0, uConstantCurrent);
        bConstantCurrentChanged = false;
    }
    return;
}

uint32_t ConstantCurrentGStrategyGetSchedule() {
    return  500U;
}

eload_mode ConstantCurrentStrategyGetMode () {
    return ELOAD_MODE_CURRENT;
}

int8_t ConstantCurrentStrategyGetChar() {
    return 'I';
}

void ConstantCurrentStrategySetCurrent(uint32_t uValue) {
    // todo: set the target current via DAC.
    // todo : convert volt/DAC settings
    // todo: float?
    uConstantCurrent = uValue;
    bConstantCurrentChanged = true;

}

void setConstantCurrentStrategy() {
    uConstantCurrent = 0U;
    bConstantCurrentChanged = true;
    __setControlStrategy(ConstantCurrentStrategyControlFunction,
                         ConstantCurrentGStrategyGetSchedule,
                         ConstantCurrentStrategyGetMode,
                         ConstantCurrentStrategyGetChar,
                         ConstantCurrentStrategySetCurrent,
                         NULL);

}


