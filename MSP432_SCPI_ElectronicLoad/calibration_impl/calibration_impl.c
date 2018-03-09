/*
 * calibration_impl.c
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */


#include "calibration_impl.h"
#include <rom.h>
#include <rom_map.h>
#include <math.h>
#include "flash.h"
#include "adc_impl.h"

#define CALIBRATION_START 0x0003F000

// update the version only when adding new fields
#define CALIBRATION_DATA_VERSION 1U

typedef struct CalibrationData {
    uint32_t version;
    float temperature_threshold;  // todo convert to the ADC 16 bit value in stead of float
    float sense_voltage_read_multiplier;
    float sense_voltage_read_offset;
    float current_read_multiplier;
    float current_read_offset;
    float current_write_multiplier;
    float current_write_offset;
} CalibrationData;

#define CALIBRATION_DATA_SIZE (sizeof(CalibrationData))

CalibrationData _CalibrationData;

bool _bCalibrationActive = false;

void calibrationWrite();


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

void calibrationRead() {
    memcpy(&_CalibrationData, (const void *)CALIBRATION_START, CALIBRATION_DATA_SIZE);
}

void calibrationWrite() {
    _CalibrationData.version = CALIBRATION_DATA_VERSION; // writing calibration data will always set the version to the latest

    /* Unprotecting Info Bank 0, Sector 0  */
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR31);

    /* Trying to erase the sector. Within this function, the API will
        automatically try to erase the maximum number of tries. If it fails,
         trap in an infinite loop */
    if(!MAP_FlashCtl_eraseSector(CALIBRATION_START))
        while(1);

    /* Trying to program the memory. Within this function, the API will
        automatically try to program the maximum number of tries. If it fails,
        trap inside an infinite loop */
    if(!MAP_FlashCtl_programMemory(&_CalibrationData,
            (void *)CALIBRATION_START, CALIBRATION_DATA_SIZE))
                while(1);

    /* Setting the sector back to protected  */
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR31);
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
    fRetval = isnan(_CalibrationData.sense_voltage_read_multiplier) ? 0.33 : _CalibrationData.sense_voltage_read_multiplier;
    return fRetval > 0.0 ? fRetval : 0.33;
}

bool calibrationSetSenseVoltageReadOffset(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.sense_voltage_read_offset = value;
    }
    return _bCalibrationActive;
}

float calibrationGetSenseVoltageReadOffset() {
    float fRetval = 0.0;
    fRetval = isnan(_CalibrationData.sense_voltage_read_offset) ? 0.0 : _CalibrationData.sense_voltage_read_offset;
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
    // The opamp U3C has a gain of -6.8, U3B -1. !! populate R32 with a 68K resistor
    float fRetval = 0.0;
    fRetval = isnan(_CalibrationData.current_read_multiplier) ? 6.8 : _CalibrationData.current_read_multiplier;
    return fRetval > 0.0 ? fRetval : 6.8;
}

bool calibrationSetCurrentReadOffset(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.current_read_offset = value;
    }
    return _bCalibrationActive;
}

float calibrationGetCurrentReadOffset() {
    float fRetval = 0.0;
    fRetval = isnan(_CalibrationData.current_read_offset) ? 0.0 : _CalibrationData.current_read_offset;
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
    fRetval = isnan(_CalibrationData.current_write_multiplier) ? 0.000116 : _CalibrationData.current_write_multiplier;
    return fRetval > 0.0 ? fRetval : 0.000116;
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
    fRetval = isnan(_CalibrationData.current_write_offset) ? 0.008 : _CalibrationData.current_write_offset;
    return fRetval;
}
