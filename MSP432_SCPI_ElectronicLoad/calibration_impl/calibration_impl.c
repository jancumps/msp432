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
    float sence_voltage_multiplier;
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

bool calibrationSetSenseVoltageMultiplier(float value) {
    if(_bCalibrationActive) {
        _CalibrationData.sence_voltage_multiplier = value;
    }
    return _bCalibrationActive;
}


float calibrationGetSenseVoltageMultiplier() {
    return isnan(_CalibrationData.sence_voltage_multiplier) ? 0.0 : _CalibrationData.sence_voltage_multiplier;
}
