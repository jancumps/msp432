/*
 * calibration_impl.h
 *
 *  Created on: 23 dec. 2017
 *      Author: jancu
 */

#ifndef CALIBRATION_IMPL_CALIBRATION_IMPL_H_
#define CALIBRATION_IMPL_CALIBRATION_IMPL_H_

#include <stdint.h>

// update the version only when adding new fields
#define CALIBRATION_DATA_VERSION 1U

typedef struct CalibrationData {
    uint32_t version;
    uint32_t temperature_thresholds[4];
} CalibrationData;

#define CALIBRATION_DATA_SIZE (sizeof(CalibrationData))

void calibrationRead();

#endif /* CALIBRATION_IMPL_CALIBRATION_IMPL_H_ */
