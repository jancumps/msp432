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



    while (1) {
        Task_sleep((UInt)arg0);
        GrClearDisplay(&g_sContext);
        switch (eloadGetMode()) {
        case ELOAD_MODE_CURRENT:
            GrStringDraw(&g_sContext, "mode: I", -1, 5, 0, 0);
            break;
        default:
            GrStringDraw(&g_sContext, "mode: not implemented", -1, 5, 0, 0);
            break;
         }

        for (i = 0; i < 4; i++) {
            // for demo purpose, show ADC voltages
            sprintf(formatString, "ADC%i: %02.4f\0", i + 1, adcImplToFloat(adcImplGetAdc(i)));
            GrStringDraw(&g_sContext, (int8_t *)formatString, -1, 4, (15 + 12*i), 0);
        }
        GrFlush(&g_sContext);

    }
}
