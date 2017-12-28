/*
 * eload_api.h
 *
 *  Created on: 24 dec. 2016
 *      Author: jancu
 */

#ifndef ELOAD_API_ELOAD_API_H_
#define ELOAD_API_ELOAD_API_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum eload_mode {
    ELOAD_MODE_CURRENT = 0,
    ELOAD_MODE_VOLTAGE,
    ELOAD_MODE_POWER,
    ELOAD_MODE_RESISTANCE,
    ELOAD_MODE_COUNT
}eload_mode;

/*!
 *  @brief  Retrieve voltage sampled from the instrument terminals
 *
 *  This function returns the most recent sampled voltage on the instruments main terminals.
 *  Calling this function doesn't initiate a sample. There's a scheduled task that collects
 *  the info in the background.
 *
e.
 */

void eloadInit();
void eloadReset();

float eloadGetVoltageDC();

void eloadSetMode(eload_mode mode);
eload_mode eloadGetMode();
int8_t eloadGetChar();
void eloadControlLoop();
uint32_t eloadControlLoopGetSchedule();

// instrument operations
bool eloadSetConstantCurrent(uint32_t value);
bool eloadSetConstantVoltage(uint32_t value);

uint32_t eloadGetCurrentRangeMax();
uint32_t eloadGetVoltageRangeMax();
uint32_t eloadGetOutputRangeMax();

void eloadInputEnable(bool bEnable);
bool eloadInputEnabled();


void eloadTest();
void eloadRawSetDac(uint32_t uModule, uint32_t value);
uint32_t eloadDevelopGetAdc(uint32_t uModule);
uint32_t eloadDevelopGetAdcRaw();


// calibration
void eloadCalibrationStart();
bool eloadCalibrationEnd();
bool eloadCalibrateSetTemperatureMaxResistance(uint32_t value);
uint32_t eloadCalibrateGetTemperatureMaxResistance();
bool eloadCalibrateSetSenseVoltMultiplier(float value);
float eloadCalibrateGetSenseVoltMultiplier();
float eloadCalibrateGetAdcVolt(uint32_t uModule);




#endif /* ELOAD_API_ELOAD_API_H_ */
