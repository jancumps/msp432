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


typedef uint32_t (*ControlStrategyControlFunction) (uint32_t uTarget);
typedef eload_mode (*ControlStrategyGetMode) ();
typedef int8_t (*ControlStrategyGetChar) ();

typedef struct ControlStrategy {
    ControlStrategyControlFunction controlFunction;
    ControlStrategyGetMode getMode;
    ControlStrategyGetChar getChar;
} ControlStrategy;


ControlStrategy *getControlStrategy();

// this is supposed to be used by the implementation classes only, not public
void __setControlStrategy(ControlStrategyControlFunction f, ControlStrategyGetMode m, ControlStrategyGetChar c);


#endif /* CONTROL_IMPL_CONTROL_STRATEGY_INTERFACE_H_ */
