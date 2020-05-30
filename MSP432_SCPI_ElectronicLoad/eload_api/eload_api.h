/**
* \file eload_api.h
* \brief public eload API
* \details Definition of all public functions for the electronic load..
* \author Jan Cumps
* \date 24 dec 2016
*/

#ifndef ELOAD_API_ELOAD_API_H_
#define ELOAD_API_ELOAD_API_H_

#include <stdint.h>
#include <stdbool.h>


typedef enum eload_mode {
    ELOAD_MODE_CURRENT = 0,//!< constant current mode
    ELOAD_MODE_VOLTAGE,    //!< constant voltage mode
    ELOAD_MODE_POWER,      //!< constant power mode
    ELOAD_MODE_RESISTANCE, //!< constant resistance mode
    ELOAD_MODE_COUNT       //!< number of modes
}eload_mode;

void eloadInit();
void eloadReset();

/**
 *  Retrieve voltage sampled from the instrument terminals
 *
 *  This function returns the most recent sampled voltage on the instruments main terminals.
 *  Calling this function doesn't initiate a sample. There's a scheduled task that collects
 *  the info in the background.
 *
 *  This represents the voltage on the sense inputs, as sampled by ADC 2.
 *  The opamp U3D has a gain of .033.
 *  The calibrated multiplier to calculate the voltage at the sense inputs
 *  is stored in the calibration structure.
 *  We fetch that in this function. If not set yet, we use the theoretical multiplier 33.3333
 *  @return most recent sampled voltage
 *
 */
float eloadGetVoltageDC();
/**
 *  Return the current on the sense resistor, as sampled by ADC 1.
 *  The opamp U3C has a gain of 7.8, U3B -1. !! populate R32 with a 68K resistor
 *  @return most recent sampled current
 *
 */
float eloadGetCurrentDC();

void eloadSetMode(eload_mode mode);
eload_mode eloadGetMode();
int8_t eloadGetChar();
void eloadControlLoop();
uint32_t eloadControlLoopGetSchedule();

// instrument operations
bool eloadSetConstantCurrent(float value);
bool eloadSetConstantVoltage(float value);
float eloadGetSetpoint();

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
bool eloadCalibrationErase();
/**
 * This function expects the resistance of the NTC thermistor at the point where the electronic load is overheated.
 * It then calculates, based on the fact that the voltage is 5V and that the NTC is the lower part of a voltage divider
 * where the other resistor is 10K, the voltage that appears on ADC 3 if the maximum temperature is reached
 * @param value the NTC cutoff resistance
 * @return true if in calibration mode. false if executed while the instrument was not in calibration mode.
 */
bool eloadCalibrateSetTemperatureMaxResistance(uint32_t value);
uint32_t eloadCalibrateGetTemperatureMaxResistance();
bool eloadCalibrateSetSenseVoltReadMultiplier(float value);
float eloadCalibrateGetSenseVoltReadMultiplier();
bool eloadCalibrateSetSenseVoltReadOffset(float value);
float eloadCalibrateGetSenseVoltReadOffset();
bool eloadCalibrateSetCurrentReadMultiplier(float value);
float eloadCalibrateGetCurrentReadMultiplier();
bool eloadCalibrateSetCurrentReadOffset(float value);
float eloadCalibrateGetCurrentReadOffset();
bool eloadCalibrateSetCurrentWriteMultiplier(float value);
float eloadCalibrateGetCurrentWriteMultiplier();
bool eloadCalibrateSetCurrentWriteOffset(float value);
float eloadCalibrateGetCurrentWriteOffset();
float eloadCalibrateGetAdcVolt(uint32_t uModule);
bool eloadCalibrationSetSenseResistance(float value);
float eloadCalibrationGetSenseResistance();

#endif /* ELOAD_API_ELOAD_API_H_ */
