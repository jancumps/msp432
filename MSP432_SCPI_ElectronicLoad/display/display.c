/*
 * display.c
 *
 *  Created on: 8 jan. 2017
 *      Author: jancu
 */
// for printf
#include <stdio.h>
#include <unistd.h>

#include <ti/display/DisplayExt.h>



#include "eload_api.h"
#include "adc_impl.h"

#include "rtos_schedules.h"



/*
 *  ======== threadDisplay ========
 *  Handle (the optional) LCD display.
 */
void *threadDisplay(void *arg0)
{
    uint32_t i;
    char formatString[12];
    Display_init();
    /* Initialize display and try to open LCD type of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;
//    Display_Handle hLcd = Display_open(Display_Type_LCD | Display_Type_GRLIB, &params);
    Display_Handle hLcd = Display_open(Display_Type_LCD, &params);
    if (!hLcd) {
        return NULL; // we didn't get a display handle. Check if the following is defined: BOARD_DISPLAY_USE_LCD
    }
    Graphics_Context *context = DisplayExt_getGraphicsContext(hLcd);
    GrContextForegroundSet(context, ClrBlack);
    GrContextBackgroundSet(context, ClrWhite);
//    GrContextForegroundSet(context, ClrWhite);
//    GrContextBackgroundSet(context, ClrBlack);

    GrClearDisplay(context);
    GrContextFontSet(context, &g_sFontCmss12);


    GrStringDraw(context, "The Breadboard", -1, 10, 15, 0);
    GrStringDraw(context, "Electronic Load", -1, 15, 30, 0);
    GrStringDraw(context, "Copyright Free", -1, 13, 45, 0);
    GrStringDraw(context, "Version 0", -1, 22, 60, 0);

    GrFlush(context);
    int8_t cMode;

    float fVoltage = 0.0f;
    float fCurrent = 0.0f;


    while (1) {
        sleep(THREAD_SLEEP_DISPLAY);
        cMode = eloadGetChar();
        fVoltage = eloadGetVoltageDC();
        fCurrent = eloadGetCurrentDC();
        GrClearDisplay(context);
        GrContextFontSet(context, &g_sFontCmss12);


        sprintf(formatString, "mode: %c %02.4f\0", cMode, eloadGetSetpoint());
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, 0, 0);

        i = 0;
        sprintf(formatString, "I: %02.4f\0", fCurrent);
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 1;
        sprintf(formatString, "V: %02.4f\0", fVoltage);
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 2;
        sprintf(formatString, "P: %02.4f\0", fVoltage * fCurrent);
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 3;
        if (fCurrent> 0.0f) {
            sprintf(formatString, "R: %02.4f\0", fVoltage / fCurrent);
        } else {
            sprintf( formatString, "R: --.--");
        }
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

        i = 4;
        sprintf(formatString, "Input: O%s\0", eloadInputEnabled()? "n " : "ff");
        GrContextFontSet(context, &g_sFontFixed6x8);
        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

//        // temperature
//        i = 2;
//        sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//        GrContextFontSet(context, &g_sFontFixed6x8);
//        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);

//        // ADC 4 is not used
//        i = 3;
//        sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//        GrContextFontSet(context, &g_sFontFixed6x8);
//        GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);


//        for (i = 0; i < 4; i++) {
//            // for demo purpose, show ADC voltages
//            sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
//            GrContextFontSet(context, &g_sFontFixed6x8);
//            GrStringDraw(context, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);
//        }
        GrFlush(context);

    }
}
