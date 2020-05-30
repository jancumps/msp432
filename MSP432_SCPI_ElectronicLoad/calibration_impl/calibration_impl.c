/*
 * calibration_impl.c
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */


#include "calibration_impl.h"
#include <math.h>
#include <string.h>
#include <ti/devices/msp432e4/driverlib/eeprom.h>
#include <ti/devices/msp432e4/driverlib/sysctl.h>
//#include "adc_impl.h"

// update the version only when adding new fields
// when changing the version number, update the compatibility section in _calibrationStructureBackwardCompatibility()
#define CALIBRATION_DATA_VERSION 2U
#define CALIBRATION_START 0x00

typedef struct CalibrationData {
    char code[6];
    uint32_t version;
    float temperature_threshold;  // todo convert to the ADC 16 bit value in stead of float
    float sense_voltage_read_multiplier;
    float sense_voltage_read_offset;
    float current_read_multiplier;
    float current_read_offset;
    float current_write_multiplier;
    float current_write_offset;
    float sense_resistance;
} CalibrationData;

#define CALIBRATION_DATA_SIZE (sizeof(CalibrationData))

CalibrationData _CalibrationData;

bool _bCalibrationActive = false;

void calibrationWrite();
void calibrationInit();


bool calibrationActive() {
    return _bCalibrationActive;
}

void calibrationStart() {
    _bCalibrationActive = true;
    calibrationRead();
}

bool calibrationEnd() {
    bool bRetVal = false;
    if (_bCalibrationActive) {
        calibrationWrite();
        _bCalibrationActive = false;
        bRetVal = true;
    }
    return (bRetVal);
}

bool calibrationErase() {
    bool bRetVal = false;
    if (_bCalibrationActive) {
        strcpy(_CalibrationData.code, "eload");
        _CalibrationData.version = CALIBRATION_DATA_VERSION;
        _CalibrationData.temperature_threshold = 0.246244535;
        _CalibrationData.sense_voltage_read_multiplier = 0.33;
        _CalibrationData.sense_voltage_read_offset = 0.0;
        _CalibrationData.current_read_multiplier = 6.8;
        _CalibrationData.current_read_offset = 0.0;
        _CalibrationData.current_write_multiplier = 0.000116;
        _CalibrationData.current_write_offset = 0.008;
        _CalibrationData.sense_resistance = 0.05; //  Ohm
        bRetVal = true;
    }
    return (bRetVal);
}



void calibrationInit() {
    static bool bInited = false;
    if(!bInited) {
        /* Enable the EEPROM peripheral. */
        SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

        /* Initialize the EEPROM */
        EEPROMInit();
        bInited = true;
    }
}

void _calibrationStructureBackwardCompatibility() {
    // compatibility with older versions of calibration structure
    if(_CalibrationData.version < CALIBRATION_DATA_VERSION) {
        if(_CalibrationData.version < 2U) { // initialise fields added in version 2 of the calibration structure
            _CalibrationData.sense_resistance = 0.05;
        }
    }
}

void calibrationRead() {
    calibrationInit();
    EEPROMRead((uint32_t *)&_CalibrationData, (uint32_t)(CALIBRATION_START),
               (uint32_t)CALIBRATION_DATA_SIZE);
    if(strcmp(_CalibrationData.code, "eload")) { // check if code == "eload"s
        calibrationErase();
    }

    // compatibility with older versions of calibration structure
    _calibrationStructureBackwardCompatibility();
}

void calibrationWrite() {
    _CalibrationData.version = CALIBRATION_DATA_VERSION; // writing calibration data will always set the version to the latest
    strcpy(_CalibrationData.code, "eload");

    calibrationInit();
    EEPROMProgram((uint32_t *)&_CalibrationData, (uint32_t)(CALIBRATION_START),
                  (uint32_t)CALIBRATION_DATA_SIZE);
}

/**
 * this function expects the resistance of the NTC thermistor at the point where the electronic load is overheated.
 * It then calculates, based on the fact that the voltage is 5V and that the NTC is the lower part of a voltage divider
 * where the other resistor is 10K, the voltage that appears on ADC 3 if the maximum temperature is reached
 */
bool calibrationSetTemperatureMaxResistance(uint32_t value) {
    if(_bCalibrationActive) {
        // formula: Vout = Vin*Rt/(R1+Rt)
        // https://en.wikipedia.org/wiki/Voltage_divider
        _CalibrationData.temperature_threshold = ((5.0*value)/(10000.0+value)); // todo convert to the ADC 16 bit value in stead of float
    }
    return _bCalibrationActive;
}

/**
 * this function returns the resistance of the NTC thyristor at the point where the electronic load is overheated.
 *
 */
uint32_t calibrationGetTemperatureMaxResistance() {
    uint32_t uRetVal = 0U;
    // formula: Rt = R1.(1/((Vin/Vout)-1))
    // https://en.wikipedia.org/wiki/Voltage_divider
    uRetVal = 10000.0 * (1.0/((5.0/_CalibrationData.temperature_threshold)-1.0)); // todo convert to the ADC 16 bit value in stead of float
    return uRetVal;
}

float calibrationGetTemperatureMaxResistanceFloat() { // todo: remove when we turned all overload functionality to uint
    return _CalibrationData.temperature_threshold;
}

// sense voltage calibration


bool calibrationSetSenseVoltageReadMultiplier(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.sense_voltage_read_multiplier = value;
    }
    return _bCalibrationActive;
}

float calibrationGetSenseVoltageReadMultiplier() {
    /* The opamp U3D has a gain of .033.
    * The calibrated multiplier to calculate the voltage at the sense inputs
    * is stored in the calibration structure.
    * We fetch that in this function. If not set yet, we use the theoretical multiplier 33.3333
    */
    float fRetval = 0.0;
    fRetval = _CalibrationData.sense_voltage_read_multiplier;
    return fRetval;
}

bool calibrationSetSenseVoltageReadOffset(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.sense_voltage_read_offset = value;
    }
    return _bCalibrationActive;
}

float calibrationGetSenseVoltageReadOffset() {
    float fRetval = 0.0;
    fRetval = _CalibrationData.sense_voltage_read_offset;
    return fRetval;
}


// current read calibration

bool calibrationSetCurrentReadMultiplier(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.current_read_multiplier = value;
    }
    return _bCalibrationActive;
}

float calibrationGetCurrentReadMultiplier() {
    // The opamp U3C has a gain of 7.8, U3B -1. !! populate R32 with a 68K resistor
    float fRetval = 0.0;
    fRetval = _CalibrationData.current_read_multiplier;
    return fRetval > 0.0 ? fRetval : 7.8;
}

bool calibrationSetCurrentReadOffset(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.current_read_offset = value;
    }
    return _bCalibrationActive;
}

float calibrationGetCurrentReadOffset() {
    float fRetval = 0.0;
    fRetval = _CalibrationData.current_read_offset;
    return fRetval;
}

// current write calibration

bool calibrationSetCurrentWriteMultiplier(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.current_write_multiplier = value;
    }
    return _bCalibrationActive;
}

float calibrationGetCurrentWriteMultiplier() {
    // The DAC drives the FET todo in my case the DAC setting multiplied by 0.000116 gives current
    float fRetval = 0.0;
    fRetval = _CalibrationData.current_write_multiplier;
    return fRetval;
}

bool calibrationSetCurrentWriteOffset(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.current_write_offset = value;
    }
    return _bCalibrationActive;
}

float calibrationGetCurrentWriteOffset() {
    // The DAC drives the FET todo in my case the DAC runs 0.008 A when not driven
    float fRetval = 0.0;
    fRetval = _CalibrationData.current_write_offset;
    return fRetval;
}

// sense resistor calibration

bool calibrationSetSenseResistance(float value) {
    bool bRetval = false;
    if(_bCalibrationActive) {
        if(value > 0.0f) {
            bRetval = true;
            _CalibrationData.sense_resistance = value;
        }
    }
    return bRetval;
}

float calibrationGetSenseResistance() {
    float fRetval = 0.0;
    fRetval = _CalibrationData.sense_resistance;
    return fRetval > 0.0 ? fRetval : 0.05;
}
