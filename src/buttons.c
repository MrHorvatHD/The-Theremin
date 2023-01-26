/*----------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------
 *      Name:    BUTTONS.H
 *      Purpose: Microprocessors Laboratory
 *----------------------------------------------------------------------------
 *
 *      Author: Pawel Russek AGH University of Science and Technology
 *---------------------------------------------------------------------------*/

#include "MKL05Z4.h" /* Device header */
#include "buttons.h"
#include "recorder.h"
#include "pit.h"

/* Buttons connected to port B */
typedef enum
{
	BUT1 = 1,
	BUT2 = 2,
	BUT3 = 0
} ButtonType;

/*Number of IRQ generated by PORTC_PORTD interrupt*/
static const IRQn_Type myPORT_IRQn = PORTB_IRQn;

/*----------------------------------------------------------------------------
	Interrupt service routine
	Button will cause PORTC_PORTD interrupt
 *----------------------------------------------------------------------------*/

void PORTB_IRQHandler(void)
{ /* Put a proper name of PORTB Interrupt service routine ISR. See startup_MKL05Z4.s file for function name */

	if (PORTB->ISFR & (1 << BUT1))
	{ /* Check in ISFR register if button BUT1 is pressed */
		if (rec_status != 2)
		{

			// if current not present start from the beginning
			if (rec_current == -1 && rec_count > 0)
				rec_current = 0;

			// play or pause the recorder
			rec_status = !rec_status;
		}

		while ((FPTB->PDIR & (1 << BUT1)) == 0)
			;								   /* Enable wait in the interrupt for SW1 button release */
		PORTB->PCR[BUT1] |= PORT_PCR_ISF_MASK; /* Make sure that interrupt service flag (ISF) in Port Control Register is cleared during ISR execution */
	}

	else if (PORTB->ISFR & (1 << BUT2))
	{ /* Check in ISFR register if button BUT2 is pressed */

		if (rec_status != 2)
		{
			rec_status = 0;
			rec_current = -1;
		}

		while ((FPTB->PDIR & (1 << BUT2)) == 0)
			;								   /* Enable wait in the interrupt for SW2 button release */
		PORTB->PCR[BUT2] |= PORT_PCR_ISF_MASK; /* Make sure that interrupt service flag (ISF) in Port Control Register is cleared during ISR execution */
	}

	else if (PORTB->ISFR & (1 << BUT3))
	{ /* Check in ISFR register if button BUT3 is pressed */

		if (rec_status == 2)
		{
			rec_status = 0;
		}
		else
		{
			clearRecorder();
			rec_status = 2;
		}

		while ((FPTB->PDIR & (1 << BUT3)) == 0)
			;								   /* Enable wait in the interrupt for SW3 button release */
		PORTB->PCR[BUT3] |= PORT_PCR_ISF_MASK; /* Make sure that interrupt service flag (ISF) in Port Control Register is cleared during ISR execution */
	}
}

/*----------------------------------------------------------------------------
	Function initializes port C pin for switch 1 (SW1) handling
	and enables PORT_C_D interrupts
 *----------------------------------------------------------------------------*/
void buttonsInitialize(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;	 /* Enable clock for port B */
	PORTB->PCR[BUT1] |= PORT_PCR_MUX(1); /* Pin PTB0 is GPIO */
	PORTB->PCR[BUT2] |= PORT_PCR_MUX(1); /* Pin PTB1 is GPIO */
	PORTB->PCR[BUT3] |= PORT_PCR_MUX(1); /* Pin PTB2 is GPIO */

	/* Port control register for bit 3 of port C configuration. Activate pull up and interrupt */
	PORTB->PCR[BUT1] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_PFE_MASK;
	PORTB->PCR[BUT1] |= PORT_PCR_IRQC(0xA);
	PORTB->PCR[BUT2] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_PFE_MASK;
	PORTB->PCR[BUT2] |= PORT_PCR_IRQC(0xA);
	PORTB->PCR[BUT3] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_PFE_MASK;
	PORTB->PCR[BUT3] |= PORT_PCR_IRQC(0xA);

	/* ARM's Nested Vector Interrupt Controller configuration */
	NVIC_ClearPendingIRQ(myPORT_IRQn);
	NVIC_EnableIRQ(myPORT_IRQn);

	NVIC_SetPriority(myPORT_IRQn, 3);
}