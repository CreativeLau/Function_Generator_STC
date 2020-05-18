#ifndef DELAY_H
#define DELAY_H

#include <intrins.h>
#include "reg51.h"
#include "config_stc.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

void Delay500us(void);
void Delay1ms(void);
void Delay5ms(void);
void Delay50ms(void);
void Delay200us();

bit Delay500ms_long_click();
bit Delay200ms_double_click();

#endif