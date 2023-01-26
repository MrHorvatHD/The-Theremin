#ifndef RECORDER_H
#define RECORDER_H

#include "frdm_bsp.h"

#define RECORD_LIMIT 500

extern char musicalTones[8][3]; 

extern uint16_t recorder[RECORD_LIMIT];
extern uint16_t rec_count;
extern uint8_t rec_status;
extern int8_t rec_current;


void insertEnd(uint16_t el); 
void clearRecorder(void);
void recorder_function(void);
   
#endif
