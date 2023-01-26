/******************************************************************************
 * This file is a part of the Sysytem Microprocessor Tutorial (C).            *
 ******************************************************************************/

/**
 * @file tpm.c
 * @author Koryciak
 * @date Nov 2020
 * @brief File containing definitions for TPM.
 * @ver 0.1
 */

#include "tpm.h"
#include "recorder.h"

#define TRIGGER_POS 9

/******************************************************************************\
* Private definitions
\******************************************************************************/
/******************************************************************************\
* Private prototypes
\******************************************************************************/
void TPM1_IRQHandler(void);
void TPM0_IRQHandler(void);
/******************************************************************************\
* Private memory declarations
\******************************************************************************/
static uint16_t tpm1Diff = 0;
static uint16_t tpm1New = 0;
static uint16_t tpm1Old = 0;
static uint8_t tpm0Enabled = 0;

static uint16_t count = 0;
static uint16_t countMax = 196;

uint8_t ton = 0;

void TPM1_Init_InputCapture(void)
{

	// set up pins for input capture
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	PORTA->PCR[TRIGGER_POS] |= PORT_PCR_MUX(1);
	PTA->PDDR |= (1 << TRIGGER_POS);
	PTA->PCOR |= (1 << TRIGGER_POS);

	// set up TPM1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[0] = PORT_PCR_MUX(2);

	TPM1->SC |= TPM_SC_PS(7);
	TPM1->SC |= TPM_SC_CMOD(1);

	TPM1->SC &= ~TPM_SC_CPWMS_MASK; /* up counting */
	TPM1->CONTROLS[0].CnSC &= ~(TPM_CnSC_MSA_MASK | TPM_CnSC_MSB_MASK);
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK); /* capture on both edges */

	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;

	NVIC_SetPriority(TPM1_IRQn, 2);

	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn); /* Enable Interrupts */
}

void TPM0_Init_PWM(void)
{

	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[7] = PORT_PCR_MUX(2);

	TPM0->SC |= TPM_SC_PS(3);
	TPM0->SC |= TPM_SC_CMOD(1);

	TPM0->MOD = 100;

	TPM0->SC &= ~TPM_SC_CPWMS_MASK;										/* up counting */
	TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK); /* set Output on match, clear Output on reload */

	TPM0->CONTROLS[2].CnV = 50;

	///////////////enabling interrupts
	// TPM0->SC &= ~TPM_SC_CPWMS_MASK;
	TPM0->CONTROLS[0].CnSC |= (TPM_CnSC_MSA_MASK | TPM_CnSC_ELSA_MASK);
	TPM0->CONTROLS[0].CnV = 0;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;

	NVIC_SetPriority(TPM0_IRQn, 1); /* TPM0 interrupt priority level  */

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn); /* Enable Interrupts */

	tpm0Enabled = 1; /* set local flag */
}

uint32_t TPM1_GetVal(void)
{
	// divide by 2 to get distance in mm
	return tpm1Diff / 2;
}

void TPM0_SetVal(uint32_t value)
{
	value = value;
	if (tpm0Enabled)
		TPM0->CONTROLS[2].CnV = value;
}

void TPM0_SetCountMax(uint8_t value)
{
	countMax = value;
}

uint8_t tones[9] = {49, 52, 58, 65, 73, 78, 88, 98};

void TPM0_SetTone(uint16_t value)
{
	// select tone depending on the distance from the ultrasonic sensor
	ton = value >= 50 && value < 260 ? (value - 20) / 30 : 0;
	countMax = tones[ton];

	// code left for better understanding
	/*if(value < 50){
		countMax=49;
		ton=8;
	}
	else if(value < 80){
		countMax=52;
		ton=7;
	}
	else if(value < 110){
		countMax=58;
		ton=6;
	}
	else if(value < 140){
		countMax=65;
		ton=5;
	}
	else if(value < 170){
		countMax=73;
		ton=4;
	}
	else if(value < 200){
		countMax=78;
		ton=3;
	}
	else if(value < 230){
		countMax=88;
		ton=2;
	}
	else if(value < 260){
		countMax=98;
		ton=1;
	}
	else{
		countMax=105;
		ton=0;
	}*/
}

/**
 * @brief Interrupt handler for TPM1.
 */
void TPM1_IRQHandler(void)
{

	tpm1Old = tpm1New;
	tpm1New = TPM1->CONTROLS[0].CnV & 0xFFFF;
	tpm1Diff = tpm1New - tpm1Old; // calculate difference

	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
}

int duty_cycles[12] = {50, 75, 93, 100, 93, 75, 50, 25, 7, 0, 7, 25};

void TPM0_IRQHandler(void)
{
	// select the right block of the sinne wave depending on the tone
	uint16_t countMax12 = countMax / 12;

	int duty_cycle_idx = count / countMax12;
	TPM0_SetVal(duty_cycles[duty_cycle_idx]);

	// code left for better understandingŁ
	/*if(count <= countMax12){
		TPM0_SetVal(50);
	}
	else if(count <= 2*countMax12){
		TPM0_SetVal(75);
	}
	else if(count <= 3*countMax12){
		TPM0_SetVal(93);
	}
	else if(count <= 4*countMax12){
		TPM0_SetVal(100);
	}
	else if(count <= 5*countMax12){
		TPM0_SetVal(93);
	}
	else if(count <= 6*countMax12){
		TPM0_SetVal(75);
	}
	else if(count <= 7*countMax12){
		TPM0_SetVal(50);
	}
	else if(count <= 8*countMax12){
		TPM0_SetVal(25);
	}
	else if(count <= 9*countMax12){
		TPM0_SetVal(7);
	}
	else if(count <= 10*countMax12){
		TPM0_SetVal(0);
	}
	else if(count <= 11*countMax12){
		TPM0_SetVal(7);
	}
	else{
		TPM0_SetVal(25);
	}*/

	count = (count >= countMax) ? 0 : count + 1;

	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
}
