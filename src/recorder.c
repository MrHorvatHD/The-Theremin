#include "tpm.h"
#include "recorder.h"

char musicalTones[8][3] = {"C5", "H4", "A4", "G4", "F4", "E4", "D4", "C4"};

uint16_t recorder[RECORD_LIMIT];
uint16_t rec_count = 0;
uint8_t rec_status = 0;
int8_t rec_current = -1;

// add tone to the end of recorder array
void insertEnd(uint16_t el)
{
	if (rec_count >= RECORD_LIMIT)
	{
		rec_status = 0;
		return;
	}

	recorder[rec_count] = el;
	rec_count++;
}

// set the recorder for a fresh start
void clearRecorder()
{
	rec_count = 0;
	rec_current = -1;
}

// main logic of the recorder
void recorder_function()
{

	// record tones
	if (rec_count <= RECORD_LIMIT && rec_status == 2)
	{
		insertEnd((int)TPM1_GetVal());
	}

	// play tones
	else if (rec_status == 1 && rec_current != -1)
	{

		TPM0_SetTone((int)recorder[rec_current]);
		rec_current++;

		// reset the recorder status
		if (rec_current >= rec_count)
		{
			rec_status = 0;
			rec_current = -1;
		}
	}
}
