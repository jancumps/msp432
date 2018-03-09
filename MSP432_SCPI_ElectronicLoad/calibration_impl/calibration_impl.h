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


bool calibrationSetSenseVoltageReadMultiplier(float value);
float calibrationGetSenseVoltageReadMultiplier();
bool calibrationSetSenseVoltageReadOffset(float value);
float calibrationGetSenseVoltageReadOffset();

bool calibrationSetCurrentReadMultiplier(float value);
float calibrationGetCurrentReadMultiplier();
bool calibrationSetCurrentReadOffset(float value);
float calibrationGetCurrentReadOffset();

bool calibrationSetCurrentWriteMultiplier(float value);
float calibrationGetCurrentWriteMultiplier();
bool calibrationSetCurrentWriteOffset(float value);
float calibrationGetCurrentWriteOffset();

#endif /* CALIBRATION_IMPL_CALIBRATION_IMPL_H_ */
