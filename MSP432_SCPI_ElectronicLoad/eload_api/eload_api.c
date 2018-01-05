/*
 * eload_api.c
 *
 *  Created on: 24 dec. 2016
 *      Author: jancu
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/cfg/global.h> // needed to get the global from the .cfg file

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>


#include <stdint.h>
#include "eload_api.h"
#include "dac_impl.h"
#include "adc_impl.h"
#include "inputenable_impl.h"

#include "control_strategy_interface.h"
#include "control_strategy_constantcurrent.h"

#include "calibration_impl.h"


void eloadInit() {
    // get the calibration data saved in Flash
    calibrationRead();
    // reset the instrument
    eloadReset();
}

void eloadReset() {
    // do not re-read calibration data in this function
    eloadSetMode(ELOAD_MODE_CURRENT);
}



/**
 * return the voltage on the sense inputs, as sampled by ADC 2.
 * The opamp U3D has a gain of .033.
 * The calibrated multiplier to calculate the voltage at the sense inputs
 * is stored in the calibration structure.
 * We fetch that in this function. If not set yet, we use the theoretical multiplier 33.3333
 */
float eloadGetVoltageDC() {
    float fRetVal = calibrationGetSenseVoltageMultiplier();
    if( fRetVal == 0.0) {
        fRetVal = 33.3333;
    }
    fRetVal = fRetVal * adcImplToFloat(adcImplGetAdc(1)); // the value is sampled from ADC 2

    return fRetVal;

}

/**
 * return the current on the sense resistor, as sampled by ADC 2.
 * The opamp U3C has a gain of -7.8, U3B -1.
 * todo: The calibrated multiplier to calculate the voltage at the sense inputs
 * is stored in the calibration structure.
 * We fetch that in this function. If not set yet, we use the theoretical multiplier 7.8
 */
float eloadGetCurrentDC() {
    float fRetVal = 7.85; // todo calibrationGetSenseCurrentMultiplier()
    if( fRetVal == 0.0) {
        fRetVal = 7.8;
    }
    fRetVal = ((adcImplToFloat(adcImplGetAdc(0) - (-1136)))*20/7.8); // the value is sampled from ADC 1 todo -1136 should be the raw adc when the current is 0; 20 (1/0.05) is the sense resistor

    return fRetVal;

}


void eloadSetMode(eload_mode mode){
    // switch input off
    if (eloadInputEnabled()) {
        eloadInputEnable(false);
    }
    switch (mode) {
    case ELOAD_MODE_CURRENT:
        setConstantCurrentStrategy();
        break;
    default:
        // don't change when an unknown mode is selected
        System_printf("Unknown or unsupported mode, ignore\n");
    }

}

eload_mode eloadGetMode() {
    return getControlStrategy()->getMode();
}

int8_t eloadGetChar() {
    return getControlStrategy()->getChar();
}

void eloadControlLoop() {
    getControlStrategy()->controlFunction();
}

uint32_t eloadControlLoopGetSchedule() {
    return getControlStrategy()->getSchedule();
}

bool eloadSetConstantCurrent(uint32_t value) {
    bool bRet = false;
    if (getControlStrategy()->setCurrent) { // does strategy support the operation?
        getControlStrategy()->setCurrent(value);
        bRet = true;
    }
    return bRet;
}


bool eloadSetConstantVoltage(uint32_t value) {
    bool bRet = false;
    if (getControlStrategy()->setVoltage) { // does strategy support the operation?
        getControlStrategy()->setVoltage(value);
        bRet = true;
    }
    return bRet;
}


uint32_t eloadGetCurrentRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eloadGetVoltageRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eloadGetOutputRangeMax() {
    return 0b1111111111111111; // 16 bit DAC
}

// these functions don't need to call the strategy because they are valid for any implementation

void eloadInputEnable(bool bEnable) {
    MsgInputEnable pMsg;

    // value has to be validated before it arrives here. We assume it's valid
    pMsg.value = bEnable;
    /* enqueue message */
    Mailbox_post(mbInputEnable, &pMsg, BIOS_WAIT_FOREVER);

}

bool eloadInputEnabled() {
    return inputenableImplGetInputEnable();
}




void eloadTest() {
    // todo: make this a system test, currently used to test different new things


}

void eloadRawSetDac(uint32_t uModule, uint32_t value) { // module is 0 based
    MsgDAC      pMsg;

    // value has to be validated before it arrives here. We assume it's valid
    pMsg.value = (uint16_t)value;
    pMsg.module = (uint8_t)uModule;
    /* enqueue message */
    Mailbox_post(mbDAC, &pMsg, BIOS_WAIT_FOREVER);

}

uint32_t eloadDevelopGetAdc(uint32_t uModule) { // module is 0 based

    return adcImplToValue(adcImplGetAdc(uModule));
}

uint32_t eloadDevelopGetAdcRaw(uint32_t uModule) { // module is 0 based

    return adcImplGetAdc(uModule);
}


void eloadCalibrationStart() {
    calibrationStart();
}

bool eloadCalibrationEnd() {
    return calibrationEnd();
}

bool eloadCalibrateSetTemperatureMaxResistance(uint32_t value) {
    return calibrationSetTemperatureMaxResistance(value);
}

uint32_t eloadCalibrateGetTemperatureMaxResistance() {
    return calibrationGetTemperatureMaxResistance();
}

bool eloadCalibrateSetSenseVoltMultiplier(float value) {
    return calibrationSetSenseVoltageMultiplier(value);
}

float eloadCalibrateGetSenseVoltMultiplier() {
    return calibrationGetSenseVoltageMultiplier();
}

float eloadCalibrateGetAdcVolt(uint32_t uModule) { // module is 0 based

    return adcImplToFloat(adcImplGetAdc(uModule));
}

