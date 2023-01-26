/******************************************************************************
 * This file is a part of the SM2 Tutorial (C).                               *                                                 *
 ******************************************************************************/

/**
 * @file tpm.h
 * @author Koryciak
 * @date Nov 2020
 * @brief File containing enums, structures and declarations for TPM.
 * @ver 0.1
 */

#ifndef TPM_H
#define TPM_H

#include "frdm_bsp.h"

extern uint8_t ton;

/**
 * @brief TPM1 initialization. Input capture.
 */
void TPM1_Init_InputCapture(void);
/**
 * @brief TPM0 initialization. PWM.
 */
void TPM0_Init_PWM(void);
/**
 * @brief Get value of counter from channel of TPM1.
 *
 * @return Value of counter.
 */
uint32_t TPM1_GetVal(void);
/**
 * @brief Set value of counter from channel of TPM0.
 *
 * @param Value of counter.
 */
void TPM0_SetVal(uint32_t value);
void TPM0_SetCountMax(uint8_t value);

void TPM0_SetTone(uint16_t value);
#endif /* TPM_H */
