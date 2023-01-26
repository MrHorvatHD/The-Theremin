#ifndef pit_h
#define pit_h

extern int counter;
extern int prevCounter;

void pitInitialize(unsigned period);
void startPIT(void);
void stopPIT(void);


#endif
