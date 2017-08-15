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


void __setControlStrategy(ControlStrategyControlFunction f,
                          ControlStrategyGetSchedule s,
                          ControlStrategyGetMode m,
                          ControlStrategyGetChar c,
                          ControlStrategySetCurrent sc,
                          ControlStrategySetVoltage sv) {

    cs.controlFunction = f;
    cs.getSchedule = s;
    cs.getMode = m;
    cs.getChar = c;
    cs.setCurrent = sc;
    cs.setVoltage = sv;
}
