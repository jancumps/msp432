/*
 * control_strategy_constantcurrent.c
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#include <control_strategy_constantcurrent.h>


uint32_t ConstantCurrentStrategyControlFunction (uint32_t uTarget) {
    // todo: set the target current via DAC.
    return uTarget;
}

eload_mode ConstantCurrentStrategyGetMode () {
    return ELOAD_MODE_CURRENT;
}

int8_t ConstantCurrentStrategyGetChar() {
    return 'I';
}

void setConstantCurrentStrategy() {
    __setControlStrategy(ConstantCurrentStrategyControlFunction, ConstantCurrentStrategyGetMode, ConstantCurrentStrategyGetChar);
}


