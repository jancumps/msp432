/*
 * Jan Cumps
 * 20161222
 * MSP432 SCPI implementation based on the SCPI parser test example of Jan Breuer
 *
 *
 */

 /**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */


/*-
 * Copyright (c) 2012-2013 Jan Breuer,
 *
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"

#include "eload_api.h"




/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
static scpi_result_t My_CoreTstQ(scpi_t * context) {

    eLoadTest();

    SCPI_ResultInt32(context, 0);

    return SCPI_RES_OK;
}

/* Electronic Load commands */

static scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {

    SCPI_ResultDouble(context, eloadGetVoltageDC());

    return SCPI_RES_OK;
}



static scpi_result_t DACGetModule(scpi_t *context, uint32_t *uModule) {
    int32_t numbers[1];

    // retrieve the DAC channel. Can be 1 - 4
    SCPI_CommandNumbers(context, numbers, 1, 1);
    if (! ((numbers[0] > 0) && (numbers[0] < 5) )) {
        SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
        return SCPI_RES_ERR;
    } else {
        *uModule = numbers[0];
        return SCPI_RES_OK;
    }
}

static scpi_result_t ADCGetModule(scpi_t *context, uint32_t *uModule) {
    int32_t numbers[1];

    // retrieve the ADC channel. Can be 1 - 4
    SCPI_CommandNumbers(context, numbers, 1, 1);
    if (! ((numbers[0] > 0) && (numbers[0] < 5) )) {
        SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
        return SCPI_RES_ERR;
    } else {
        *uModule = numbers[0];
        return SCPI_RES_OK;
    }
}



/**
 * Set DAC output directly

 *
 * Return SCPI_RES_OK
 *

 */
static scpi_result_t SCPI_DevelopSetDac(scpi_t * context) {

    uint32_t param1;
    uint32_t uModule;



    if (DACGetModule(context, &uModule) == SCPI_RES_ERR) {
        return SCPI_RES_ERR;
    }


     /* read first parameter if present */
     if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
         return SCPI_RES_ERR;
     }
     if (param1 > UINT16_MAX) {
         SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
         return SCPI_RES_ERR;
     } else {
         eLoadRawSetDac(uModule - 1, param1);
     }


    return SCPI_RES_OK;
}

static scpi_result_t SCPI_DevelopGetAdc(scpi_t * context) {

    uint32_t uModule;

    if (ADCGetModule(context, &uModule) == SCPI_RES_ERR) {
        return SCPI_RES_ERR;
    }
    SCPI_ResultUInt32(context, eLoadDevelopGetAdc(uModule - 1));

    return SCPI_RES_OK;
}

static scpi_result_t SCPI_DevelopGetAdcRaw(scpi_t * context) {

    uint32_t uModule;

    if (ADCGetModule(context, &uModule) == SCPI_RES_ERR) {
        return SCPI_RES_ERR;
    }
    SCPI_ResultUInt32(context, eLoadDevelopGetAdcRaw(uModule - 1));

    return SCPI_RES_OK;
}

static scpi_result_t SCPI_DevelopGetAdcVolt(scpi_t * context) {

    uint32_t uModule;

    if (ADCGetModule(context, &uModule) == SCPI_RES_ERR) {
        return SCPI_RES_ERR;
    }
    SCPI_ResultFloat(context, eLoadDevelopGetAdcVolt(uModule - 1));

    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_SetCurrentImmediate(scpi_t * context) {
    uint32_t param1;

        /* read first parameter if present */
    if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }
    if (param1 > UINT16_MAX) {
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
    if (!eloadSetConstantCurrent(param1) ){
        SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR); // fail if function not supported by current operation mode
        return SCPI_RES_ERR;
    }


    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_SetVoltageImmediate(scpi_t * context) {
    uint32_t param1;

        /* read first parameter if present */
    if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }
    if (param1 > UINT16_MAX) {
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }
    if (!eloadSetConstantVoltage(param1) ){
        SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR); // fail if function not supported by current operation mode
        return SCPI_RES_ERR;
    }


    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_SetInputState(scpi_t * context) {
    scpi_bool_t param1;

        /* read first parameter if present */
    if (!SCPI_ParamBool(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }
    eloadInputEnable(param1);


    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_GetInputState(scpi_t * context) {
    SCPI_ResultBool(context, eloadInputEnabled());

    return SCPI_RES_OK;
}

/* CALIBRATION AND CONFIGURATION COMMANDS */

static scpi_result_t ELOAD_CalibrationStart(scpi_t * context) {
    eloadCalibrationStart();
    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_CalibrationEnd(scpi_t * context) {
    if (!eloadCalibrationEnd()){
        SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_CalibrationSetTemperatureMaxResistance(scpi_t * context) {
    uint32_t param1;
    if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }
    if (!eLoadCalibrateSetTemperatureMaxResistance(param1)){
        SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
        return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

static scpi_result_t ELOAD_CalibrationGetTemperatureMaxResistance(scpi_t * context) {
    SCPI_ResultUInt32(context, eLoadCalibrateGetTemperatureMaxResistance());
    return SCPI_RES_OK;
}

const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = My_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},


    /* DMM */
    {.pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
//    {.pattern = "CONFigure:VOLTage:DC", .callback = DMM_ConfigureVoltageDc,},
//    {.pattern = "MEASure:VOLTage:DC:RATio?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:VOLTage:AC?", .callback = DMM_MeasureVoltageAcQ,},
//    {.pattern = "MEASure:CURRent:DC?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:CURRent:AC?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:RESistance?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:FRESistance?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:FREQuency?", .callback = SCPI_StubQ,},
//    {.pattern = "MEASure:PERiod?", .callback = SCPI_StubQ,},

    /* LOW LEVEL for DEVELOP only */
    {.pattern = "DEVElop:DAC#", .callback = SCPI_DevelopSetDac,},
    {.pattern = "DEVElop:ADC#?", .callback = SCPI_DevelopGetAdc,},
    {.pattern = "DEVElop:ADC#:RAW?", .callback = SCPI_DevelopGetAdcRaw,},
    {.pattern = "DEVElop:ADC#:VOLTage?", .callback = SCPI_DevelopGetAdcVolt,},


    /* ELECTRONIC LOAD COMMANDS */
    {.pattern = "[:SOURce]:CURRent[:LEVel][:IMMediate]", .callback = ELOAD_SetCurrentImmediate,},
    {.pattern = "[:SOURce]:VOLTage[:LEVel][:IMMediate]", .callback = ELOAD_SetVoltageImmediate,},
    {.pattern = "[:SOURce]:INPut[:STATe]", .callback = ELOAD_SetInputState,},
    {.pattern = "[:SOURce]:INPut[:STATe]?", .callback = ELOAD_GetInputState,},

    /* CALIBRATION AND CONFIGURATION COMMANDS */
    {.pattern = ":CALibration:Start", .callback = ELOAD_CalibrationStart,},
    {.pattern = ":CALibration:End", .callback = ELOAD_CalibrationEnd,},
    {.pattern = ":CALibration:TEMPERATUREMaxResistance", .callback = ELOAD_CalibrationSetTemperatureMaxResistance,},
    {.pattern = ":CALibration:TEMPERATUREMaxResistance?", .callback = ELOAD_CalibrationGetTemperatureMaxResistance,},

    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error = NULL,            // haven't implemented an error logger
    .write = SCPI_Write,
    .control = NULL,        // haven't implemented communication channel control
    .flush = NULL,            // don't need flush for SCI / USB
    .reset = NULL,            // todo haven't implemented instrument reset
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;


// init helper for this instrument
void scpi_instrument_init() {
     SCPI_Init(&scpi_context,
             scpi_commands,
             &scpi_interface,
             scpi_units_def,
             SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
             scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
             scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

}

scpi_bool_t scpi_instrument_input(const char * data, int len) {
    return SCPI_Input(&scpi_context, data, len);
}

