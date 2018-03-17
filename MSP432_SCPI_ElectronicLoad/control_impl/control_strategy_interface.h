/*
 * control_strategy_interface.h
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 *
 *      see http://www.adampetersen.se/Patterns%20in%20C%203,%20STRATEGY.pdf
 */

#ifndef CONTROL_IMPL_CONTROL_STRATEGY_INTERFACE_H_
#define CONTROL_IMPL_CONTROL_STRATEGY_INTERFACE_H_

#include <stdint.h>
#include "eload_api.h"


typedef void (*ControlStrategyControlFunction) ();
typedef uint32_t (*ControlStrategyGetSchedule) ();
typedef eload_mode (*ControlStrategyGetMode) ();
typedef int8_t (*ControlStrategyGetChar) ();
typedef void (*ControlStrategySetCurrent) (float fValue);
typedef void (*ControlStrategySetVoltage) (float fValue);
typedef float (*ControlStrategyGetSetpoint) ();


typedef struct ControlStrategy {
    ControlStrategyControlFunction controlFunction; // control loop implementation
    ControlStrategyGetSchedule getSchedule;         // number of rtos ticks to sleep until next execution
    ControlStrategyGetMode getMode;                 // what strategy is this, e.g.: ELOAD_MODE_CURRENT
    ControlStrategyGetChar getChar;                 // one char representation of the strategy. e.g.: 'I' for constant current
    ControlStrategySetCurrent setCurrent;           // set current for those strategies that support it (e.g.: constant current)
    ControlStrategySetCurrent setVoltage;           // set voltage for those strategies that support it (e.g.: constant voltage)
    ControlStrategyGetSetpoint getSetpoint;         // get the set value.
} ControlStrategy;


ControlStrategy *getControlStrategy();

// this is supposed to be used by the implementation classes only, not public
// additionally, the strategy should init itself before calling this function
void __setControlStrategy(ControlStrategyControlFunction f,
                          ControlStrategyGetSchedule s,
                          ControlStrategyGetMode m,
                          ControlStrategyGetChar c,
                          ControlStrategySetCurrent sc,
                          ControlStrategySetVoltage sv,
                          ControlStrategyGetSetpoint sp  );



#endif /* CONTROL_IMPL_CONTROL_STRATEGY_INTERFACE_H_ */
