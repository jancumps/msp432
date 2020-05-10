/*
 * rtos_schedules.h
 *
 *  Created on: 6 mei 2018
 *      Author: jancu
 */

#ifndef RTOS_SCHEDULES_RTOS_SCHEDULES_H_
#define RTOS_SCHEDULES_RTOS_SCHEDULES_H_



//done handleTaskADC
//done handleTaskControl
//done handleTaskDAC
//done handleTaskDisplay
//done handleTaskHeartbeat
//done handleTaskInputEnable
//done handleTaskTemperatureOverProtection
//done handleTaskUART



void *threadHeartBeat(void *arg0);
#define THREAD_PRIORITY_HEARTBEAT 1
#define THREAD_SLEEP_HEARTBEAT 1



void *threadUART(void *arg0);
#define THREAD_PRIORITY_UART 10
//#define THREAD_SLEEP_UART 1 -- this thread is Semaphore driven, no sleep

void *threadDisplay(void *arg0);
#define THREAD_PRIORITY_DISPLAY 10
#define THREAD_SLEEP_DISPLAY 1
#define THREAD_STACK_DISPLAY 2048

void *threadADC(void *arg0);
#define THREAD_PRIORITY_ADC 10
// sleep has to be higher than 81000 (81 ms idled when sampling 3 ADCs
// 100 ms : 100000 - 81000
#define THREAD_USLEEP_ADC 19000
// 1s : 100000 - 81000
// #define THREAD_USLEEP_ADC 919999

void *threadControl(void *arg0);
#define THREAD_PRIORITY_CONTROL 15
#define THREAD_USLEEP_CONTROL 999999

void *threadInputEnable(void *arg0);
#define THREAD_PRIORITY_INPUTENABLE 10
//#define THREAD_SLEEP_INPUTENABLE 1 -- this thread is message queue driven, no sleep

void *threadDAC(void *arg0);
#define THREAD_PRIORITY_DAC 10
//#define THREAD_SLEEP_DAC 1 -- this thread is message queue driven, no sleep


void *threadTemperatureOverProtection(void *arg0);
#define THREAD_PRIORITY_TEMPERATUREOVERPROTECTION 10
#define THREAD_SLEEP_TEMPERATUREOVERPROTECTION 1



#endif /* RTOS_SCHEDULES_RTOS_SCHEDULES_H_ */
