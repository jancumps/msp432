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

Graphics_Context g_sContext;

/*
 *  ======== fnTaskDisplay ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void fnTaskDisplay(UArg arg0, UArg arg1)
{
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




    }
}
