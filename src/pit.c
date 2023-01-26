/*----------------------------------------------------------------------------
 *      Name:    pit.c
 *
 *      Purpose: Microprocessors Laboratory 2
 *
 *      Author: Pawel Russek AGH University of Science and Technology
 *---------------------------------------------------------------------------*/

#include "MKL05Z4.h" //Device header
#include "pit.h"	 //Declarations
#include "frdm_bsp.h"
#include "tpm.h"
#include "recorder.h"

static const IRQn_Type myPIT_IRQn = PIT_IRQn;
extern int counter;

int counter = 0;
int prevCounter = 0;

#define TRIGGER_POS 9

void PIT_IRQHandler()
{

	// check to see which channel triggered interrupt
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK)
	{
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;

		// send pulse with ultrasonic sensor
		PTA->PSOR |= (1 << TRIGGER_POS);
		DELAY_MICRO(10)
		PTA->PCOR |= (1 << TRIGGER_POS);

		// Do ISR work
		// counter for main loop display timing
		counter++;
		PTA->PCOR |= (1 << TRIGGER_POS);

		// TPM0_PCM_Play();
		// TPM0_SetCountMax(TPM1_GetVal());

		if (rec_status != 1)
			TPM0_SetTone((int)TPM1_GetVal());

		// check with recorder
		recorder_function();
	}

	// clear pending IRQ
	NVIC_ClearPendingIRQ(myPIT_IRQn);
}

/*----------------------------------------------------------------------------
 Function that initializes PIT timer
*----------------------------------------------------------------------------*/
void pitInitialize(unsigned period)
{

	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;

	// Initialize PIT0 to count down from argument
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;

	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(myPIT_IRQn, 2);

	NVIC_ClearPendingIRQ(myPIT_IRQn);
	NVIC_EnableIRQ(myPIT_IRQn);
}

void startPIT(void)
{
	// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void stopPIT(void)
{
	// Disable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}
