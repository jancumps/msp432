/*
 * calibration_impl.h
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */

#ifndef CALIBRATION_IMPL_CALIBRATION_IMPL_H_
#define CALIBRATION_IMPL_CALIBRATION_IMPL_H_

#include <stdint.h>
#include <stdbool.h>

void calibrationStart();
bool calibrationEnd();

void calibrationRead();

bool calibrationSetTemperatureMaxResistance(uint32_t value);
uint32_t calibrationGetTemperatureMaxResistance();
float calibrationGetTemperatureMaxResistanceFloat();


bool calibrationSetSenseVoltageMultiplier(float value);
float calibrationGetSenseVoltageMultiplier();
bool calibrationSetSenseVoltageOffset(float value);
float calibrationGetSenseVoltageOffset();

bool calibrationSetCurrentMultiplier(float value);
float calibrationGetCurrentMultiplier();
bool calibrationSetCurrentOffset(float value);
float calibrationGetCurrentOffset();

#endif /* CALIBRATION_IMPL_CALIBRATION_IMPL_H_ */
