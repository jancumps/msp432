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

void eloadReset() {
    // do not re-read calibration data in this function
    eloadSetMode(ELOAD_MODE_CURRENT);
}



double eloadGetVoltageDC() {
    return (double)3.141592653589793; // todo: get input voltage from the sampled value
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


uint32_t eLoadGetCurrentRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eLoadGetVoltageRangeMax() {
    return 0b1111111111111111; // 16 bit ADC
}

uint32_t eLoadGetOutputRangeMax() {
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




void eLoadTest() {
    // todo: make this a system test, currently used to test different new things


}

void eLoadRawSetDac(uint32_t uModule, uint32_t value) { // module is 0 based
    MsgDAC      pMsg;

    // value has to be validated before it arrives here. We assume it's valid
    pMsg.value = (uint16_t)value;
    pMsg.module = (uint8_t)uModule;
    /* enqueue message */
    Mailbox_post(mbDAC, &pMsg, BIOS_WAIT_FOREVER);

}

uint32_t eLoadDevelopGetAdc(uint32_t uModule) { // module is 0 based

    return adcImplToValue(adcImplGetAdc(uModule));
}

uint32_t eLoadDevelopGetAdcRaw(uint32_t uModule) { // module is 0 based

    return adcImplGetAdc(uModule);
}

float eLoadDevelopGetAdcVolt(uint32_t uModule) { // module is 0 based

    return adcImplToFloat(adcImplGetAdc(uModule));
}

/**
 * return the voltage on the sense inputs, as sampled by ADC B.
 * The opamp U3D has a gain of .033.
 * The calibrated multiplier to calculate the voltage at the sense inputs
 * is stored in the calibration structure.
 * We fetch that in this function. If not set yet, we use the theoreticalmultiplier 33.3333
 */
float eLoadGetSenseVoltage() {
    float fRetVal = calibrationGetSenseVoltageMultiplier();
    if( fRetVal == 0.0) {
        fRetVal = 33.3333;
    }
    fRetVal = fRetVal * adcImplToFloat(adcImplGetAdc(1)); // thevalue is sampled from ADC B

    return fRetVal;

}


void eloadCalibrationStart() {
    calibrationStart();
}

bool eloadCalibrationEnd() {
    return calibrationEnd();
}

bool eLoadCalibrateSetTemperatureMaxResistance(uint32_t value) {
    return calibrationSetTemperatureMaxResistance(value);
}

uint32_t eLoadCalibrateGetTemperatureMaxResistance() {
    return calibrationGetTemperatureMaxResistance();
}

bool eLoadCalibrateSetSenseVoltMultiplier(float value) {
    return calibrationSetSenseVoltageMultiplier(value);
}

float eLoadCalibrateGetSenseVoltMultiplier() {
    return calibrationGetSenseVoltageMultiplier();
}
