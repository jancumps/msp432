/*
 * calibration_impl.c
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */


#include "calibration_impl.h"
#include <rom.h>
#include <rom_map.h>
#include "flash.h"

#define CALIBRATION_START 0x0003F000

// update the version only when adding new fields
#define CALIBRATION_DATA_VERSION 1U

typedef struct CalibrationData {
    uint32_t version;
    uint32_t temperature_threshold;
} CalibrationData;

#define CALIBRATION_DATA_SIZE (sizeof(CalibrationData))

CalibrationData _CalibrationData;

bool _bCalibrationActive = false;

bool calibrationActive() {
    return _bCalibrationActive;
}

void calibrationStart() {
    _bCalibrationActive = true;
    calibrationRead();
}

bool calibrationEnd() {
    if (_bCalibrationActive) {
        calibrationWrite();
        _bCalibrationActive = false;
    }
    return (! _bCalibrationActive); // success if we cleared the calibration active flag
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
 * this function expects the resistance of the NTC thyristor at the point where the electronic load is overheated.
 * It then calculates, based on the fact that the voltage is 5V and that the NTC is the lower part of a voltage divider
 * where the other resistor is 10K, the voltage that appears on ADC C if the maximum temperature is reached
 */
bool calibrationSetTemperatureMaxResistance(uint32_t value) {
    if(_bCalibrationActive) {
        // Vout formula is Vin*Rt/(R1+Rt)
        _CalibrationData.temperature_threshold = ((5*value)/(10000+value)); // todo work on the floating points here.
    }
    return _bCalibrationActive;
}

/**
 * this function returns the resistance of the NTC thyristor at the point where the electronic load is overheated.
 *
 */
uint32_t calibrationGetTemperatureMaxResistance() {
    uint32_t uRetVal = 0U;

    if(_CalibrationData.temperature_threshold) {
        // formula: Rt = R1.(1/((Vin/Vout)-1))
        uRetVal = 10000 * (1/((5/_CalibrationData.temperature_threshold)-1));

    }
    return uRetVal;
}
