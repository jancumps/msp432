/*
 * control_strategy_interface.c
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#include "control_strategy_interface.h"


static ControlStrategy cs;


ControlStrategy *getControlStrategy() {
    return &cs;
}


void __setControlStrategy(ControlStrategyControlFunction f, ControlStrategyGetMode m, ControlStrategyGetChar c) {

    cs.controlFunction = f;
    cs.getMode = m;
    cs.getChar = c;
}
