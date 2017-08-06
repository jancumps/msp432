/*
 * control_strategty_constantcurrent.h
 *
 *  Created on: 6 aug. 2017
 *      Author: jancu
 */

#ifndef CONTROL_IMPL_CONTROL_STRATEGY_CONSTANTCURRENT_H_
#define CONTROL_IMPL_CONTROL_STRATEGY_CONSTANTCURRENT_H_


#include "control_strategy_interface.h"


uint32_t ConstantCurrentStrategyControlFunction (uint32_t uTarget);
eload_mode ConstantCurrentStrategyGetMode ();

void setConstantCurrentStrategy();



#endif /* CONTROL_IMPL_CONTROL_STRATEGY_CONSTANTCURRENT_H_ */
