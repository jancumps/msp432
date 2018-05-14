/*
 * inputenable_impl.h
 *
 *  Created on: 3 dec. 2017
 *      Author: jancu
 */

#ifndef INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_
#define INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_

#include <stdbool.h>

#define QUEUE_NAME_INPUTENABLE "/inputenable_queue"

typedef struct MsgInputEnable {
     bool value;
} MsgInputEnable;

// for TI-RTOS message size, use the structure size:
#define MSGINPUTENABLE_SIZE (sizeof(MsgInputEnable))

bool inputenableImplGetInputEnable();


#endif /* INPUTENABLE_IMPL_INPUTENABLE_IMPL_H_ */
