#ifndef LCD1602_H
#define LCD1602_H

//#include "stc_header.h"
#include "delay.h"
#include "reg51.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#define LCD_INIT 0x38 				//0011 1000，初始化两行8位数据总线，5x8点阵；
#define LCD_CLEAR 0x01 				//0000 0001，清屏
#define LCD_OFF 0x08				//0000 1000，关显示
#define LCD_CURSOR_BLINK 0x0f		//0000 1111，开显示，有光标并闪烁
#define LCD_NOCURSOR 0x0c			//0000 1100，开显示，无光标
#define LCD_MODE 0x06				//0000 0110，读或写一个字符后，光标右移，指针加1
#define LCD_HOME1 0x80				//光标回到左上角
#define LCD_HOME2 (0x80+0x40)		//光标回到左下角
#define BF_COUT 100 				//检测忙碌次数

#define DataPort P1			//LCD数据和命令接口
sbit RS=P0^5;	 			//LCD数据命令选择
sbit RW=P0^6;	 			//LCD读写选择               
sbit E=P0^7;	  			//LCD使能

extern uint8 code page1_position[];
extern uint8 code page2_position[];

void Lcd_Init(void);
bit Lcd_Check_Busy(void);
void Lcd_Write_Command(uint8);
void Lcd_Write_Data(uint8);
void Lcd_Write_Str(uint8 *s);
void Lcd_Write_Int(long int i);

//void Lcd_Display_HEX(unsigned int num);
//void Lcd_Display_INT(unsigned int num);
#endif



