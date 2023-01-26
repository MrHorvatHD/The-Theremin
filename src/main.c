/*----------------------------------------------------------------------------
 *      Main: Initialize
 *---------------------------------------------------------------------------*/

#include "MKL05Z4.h" /* Device header */
#include "pit.h"
#include "buttons.h" /* Buttons of external keyboard */
#include "lcd1602.h"
#include "tpm.h"
#include "led.h"
#include "recorder.h"

int main(void)
{
	LCD1602_Init(); // LCD initialization
	LCD1602_Backlight(TRUE);
	LCD1602_ClearAll();

	// Init
	pitInitialize(20901 * 100);
	startPIT();

	TPM1_Init_InputCapture();
	TPM0_Init_PWM();
	LED_Init();
	buttonsInitialize();

	while (1)
	{
		__wfi(); // Save energy and wait for interrupt

		if (counter > prevCounter)
		{
			LCD1602_SetCursor(0, 0);
			prevCounter = counter;

			// display value of sensor
			LCD1602_PrintNum((int)TPM1_GetVal());

			// display current recorder playing time
			LCD1602_SetCursor(0, 1);
			if (rec_current >= 0)
			{
				LCD1602_PrintNum(rec_current);
			}
			else
			{
				LCD1602_Print("   ");
			}
			// display music note symbol
			LCD1602_SetCursor(3, 1);
			LCD1602_PrintCustom(3);

			// print recorder status
			LCD1602_SetCursor(11, 1);
			LCD1602_PrintCustom(rec_status);
			LCD1602_PrintNum(rec_count);

			// display the tone played
			LCD1602_SetCursor(11, 0);
			LCD1602_Print(musicalTones[ton]);
		}
	}
}
