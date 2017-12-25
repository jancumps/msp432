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

CalibrationData _CalibrationData;

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
