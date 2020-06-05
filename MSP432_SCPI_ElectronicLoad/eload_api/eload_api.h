/**
* @file eload_api.h
* @brief public eload instrument API
*
* Definition of all public functions for the electronic load.
* These functions can be used by user interface and remote access to
* control and poll the instrument.
*
* @author Jan Cumps
* @date 24 dec 2016
*/

#ifndef ELOAD_API_ELOAD_API_H_
#define ELOAD_API_ELOAD_API_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Supported (and at this time unsupported) operation modes
 *
 * The different modes the load can operate in.
 * Each mode is implemented by a strategy in the control_impl module.
 */
typedef enum eload_mode {
    ELOAD_MODE_CURRENT = 0,//!< constant current mode
    ELOAD_MODE_VOLTAGE,    //!< constant voltage mode. Unsupported
    ELOAD_MODE_POWER,      //!< constant power mode. Unsupported
    ELOAD_MODE_RESISTANCE, //!< constant resistance mode. Unsupported
    ELOAD_MODE_COUNT       //!< number of modes
}eload_mode;

/**
 * @brief Get calibration values and initialise the device
 *
 * The function reads calibration settings from persistent memory. It then calls eloadReset()
 * to initialise the instrument.
 */
void eloadInit();

/**
 * @brief Initialise the instrument before use
 *
 * The function sets all instrument modules in their initial state.
 * It does not load calibration data from persistent storage.
 * The calibration settings in RAM are not changed.
 * Input is switched off, and eloadSetMode() is called
 * to set the device in constant current mode.
 */
void eloadReset();

/**
 *  @brief Retrieve voltage sampled from the instrument terminals
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
 *  @brief Retrieve current through the sense resistor
 *
 *  Return the current on the sense resistor, as sampled by ADC 1.
 *  The opamp U3C has a gain of 7.8, U3B -1. (switch to 6.8 if R32 populated with a 68K resistor)
 *  @return most recent sampled current
 *
 */
float eloadGetCurrentDC();

/**
 * @brief Set the instrument operation mode
 *
 * You call this function when you want to put the instrument in one of the supported operation modes.
 * This function switches the input off, because the settings of the new mode aren't know at switching time.
 * @param mode the new operation mode
 */
void eloadSetMode(eload_mode mode);

/**
 * @brief Get the active instrument mode
 *
 * This function returns the mode the instrument is currently in.
 * @return the active operation mode
 */
eload_mode eloadGetMode();

/**
 * @brief Get the active instrument mode, as a single char
 *
 * This function returns the mode the instrument is currently in its 1-char representation.
 * E.g.: "I" in constan current mode.
 * This is used on the display.
 * @return the active operation mode
 */
int8_t eloadGetChar();

/**
 * @brief Execute control loop
 *
 * For some control strategies (any but constant current) the loop is done in software.
 * It's usuall called from an RTOS task.
 * In constant current mode, regulation is done in hardware, and there this loop does nothing.
 */
void eloadControlLoop();

/**
 * @brief The desired schedule for the active control strategy
 *
 * Some control strategies require fast repeat of execution.
 * Others (in particular constant current) don't need the control loop to be called
 * because the mode is not software controlled.
 * Each strategy can inform RTOS what the ideal callback frequency is (in microseconds).
 * @return requested callback schedule in us
 */
uint32_t eloadControlLoopGetSchedule();

// instrument operations
/**
 * @brief Ask active instrument mode to set constant current level
 *
 * Request the instrument to maintain constant current.
 * Depending on the instrument mode, this may be supported or not.
 * If it's supported, the instrument will from now on try to maintain the requested constant current.
 * @param value Requested constant current to maintain.
 * @return true if the active mode supports constant current. Else false.
 */
bool eloadSetConstantCurrent(float value);

/**
 * @brief Ask active instrument mode to set constant voltage level
 *
 * Request the instrument to maintain constant voltage.
 * Depending on the instrument mode, this may be supported or not.
 * If it's supported, the instrument will from now on try to maintain the requested constant voltage.
 * @param value Requested constant voltage to maintain.
 * @return true if the active mode supports constant voltage. Else false.
 */bool eloadSetConstantVoltage(float value);

 /**
  * @brief Get the regulation point that the instrument was set to
  *
  * Retrieve the regulation point. The unit of measure is dependent on the active mode of the instrument.
  * If it's in constant current mode, the UoM will be Ampere. In constant voltage mode in Volt. Etc ...
  * @return the regulation value that the instrument is set to.
  */
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
 * @brief Set the resistance of the NTC where the overheat protection should kick in
 *
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
