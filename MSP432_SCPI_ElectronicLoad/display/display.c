/*
 * display.c
 *
 *  Created on: 8 jan. 2017
 *      Author: jancu
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <lcddriver/sharp96x96.h>

#include "eload_api.h"
#include "adc_impl.h"

// for printf
#include <stdio.h>

Graphics_Context g_sContext;

/*
 *  ======== fnTaskDisplay ========
 *  Handle (the optional) LCD display. The Task_sleep is determined by arg0 which
 *  is configured for the fnTaskDisplay Task instance.
 */
Void fnTaskDisplay(UArg arg0, UArg arg1)
{
    uint32_t i;
    char formatString[12];


    Sharp96x96_LCDInit();
    GrContextInit(&g_sContext, &g_sharp96x96LCD);
    GrContextForegroundSet(&g_sContext, ClrBlack);
    GrContextBackgroundSet(&g_sContext, ClrWhite);

    GrClearDisplay(&g_sContext);
    GrContextFontSet(&g_sContext, &g_sFontCmss12);

    GrStringDraw(&g_sContext, "The Breadboard", -1, 10, 15, 0);
    GrStringDraw(&g_sContext, "Electronic Load", -1, 15, 30, 0);
    GrStringDraw(&g_sContext, "Copyright Free", -1, 13, 45, 0);
    GrStringDraw(&g_sContext, "Version 0", -1, 22, 60, 0);

    GrFlush(&g_sContext);
    int8_t cMode;

    float fVoltage = 0.0f;
    float fCurrent = 0.0f;


    while (1) {
        Task_sleep((UInt)arg0);
        cMode = eloadGetChar();
        fVoltage = eloadGetVoltageDC();
        fCurrent = eloadGetCurrentDC();
        GrClearDisplay(&g_sContext);
        GrContextFontSet(&g_sContext, &g_sFontCmss12);


//        GrStringDraw(&g_sContext, "mode: ", -1, 5, 0, 0);
//        GrStringDraw(&g_sContext, &cMode, 1, 40, 0, 0);
        sprintf(formatString, "mode: %c %02.4f\0", cMode, eloadGetSetpoint());
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, 0, 0);

        i = 0;
        sprintf(formatString, "I: %02.4f\0", fCurrent);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 1;
        sprintf(formatString, "V: %02.4f\0", fVoltage);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 2;
        sprintf(formatString, "P: %02.4f\0", fVoltage * fCurrent);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 3;
        if (fCurrent> 0.0f) {
            sprintf(formatString, "R: %02.4f\0", fVoltage / fCurrent);
        } else {
            sprintf( formatString, "R: --.--");
        }
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 4;
        sprintf(formatString, "Input: O%s\0", eloadInputEnabled()? "n " : "ff");
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

//        // temperature
//        i = 2;
//        sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
//        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

//        // ADC 4 is not used
//        i = 3;
//        sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
//        GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);


//        for (i = 0; i < 4; i++) {
//            // for demo purpose, show ADC voltages
//            sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//            GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
//            GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);
//        }
        GrFlush(&g_sContext);

    }
}
