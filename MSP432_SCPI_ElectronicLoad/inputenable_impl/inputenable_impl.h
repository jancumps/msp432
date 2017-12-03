/*
 * outputenable_impl.h
 *
 *  Created on: 3 dec. 2017
 *      Author: jancu
 */

#ifndef INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_
#define INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_

#include <stdbool.h>

// for TI-RTOS message size, count the bytes. Currently: 1
typedef struct MsgInputEnable {
     bool value;
} MsgInputEnable;

bool inputenableImplGetInputEnable();


#endif /* INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_ */
