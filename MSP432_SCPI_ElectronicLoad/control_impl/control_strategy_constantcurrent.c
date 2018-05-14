/*
 * control_strategy_constantcurrent.c
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#include "control_strategy_constantcurrent.h"
#include "eload_api.h"
#include "calibration_impl.h"
#include <stddef.h>
#include <stdbool.h>

float fConstantCurrent;
bool bConstantCurrentChanged;

void ConstantCurrentStrategyControlFunction () {
    if (bConstantCurrentChanged) {
        uint32_t value = 0U;
        value = ((fConstantCurrent/calibrationGetCurrentWriteMultiplier())-calibrationGetCurrentWriteOffset()); // todo: double check if it is - or plus offset
        eloadRawSetDac(0, value);
        bConstantCurrentChanged = false;
    }
    return;
}

uint32_t ConstantCurrentGStrategyGetSchedule() {
    return  999999U;
}

eload_mode ConstantCurrentStrategyGetMode () {
    return ELOAD_MODE_CURRENT;
}

int8_t ConstantCurrentStrategyGetChar() {
    return 'I';
}

void ConstantCurrentStrategySetCurrent(float fValue) {
    fConstantCurrent = fValue;
    bConstantCurrentChanged = true;
}

float ConstantCurrentStrategyGetSetpoint() {
    return fConstantCurrent;
}

void setConstantCurrentStrategy() {
    fConstantCurrent = 0.0f;
    bConstantCurrentChanged = true;
    __setControlStrategy(ConstantCurrentStrategyControlFunction,
                         ConstantCurrentGStrategyGetSchedule,
                         ConstantCurrentStrategyGetMode,
                         ConstantCurrentStrategyGetChar,
                         ConstantCurrentStrategySetCurrent,
                         NULL,
                         ConstantCurrentStrategyGetSetpoint);

}


