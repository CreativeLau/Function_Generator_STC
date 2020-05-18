/*
	lcd1602.c
	
    Driver for LCD1602

	GitHub: https://github.com/CreativeLau/Function_Generator_STC
	
	Copyright (c) 2020 Creative Lau (CreativeLauLab@gmail.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/


#include "lcd1602.h"
#include <intrins.h>
uint8 code rec1[] = {0x00, 0x00, 0x0E, 0x0A, 0x0A, 0x1B, 0x00, 0x00};
//uint8 code rec2[]={0x00,0x00,0x0F,0x09,0x09,0x19,0x00,0x00};
uint8 code sin1[] = {0x00, 0x00, 0x0E, 0x11, 0x00, 0x00, 0x00, 0x00};
uint8 code sin2[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x0E, 0x00, 0x00};
uint8 code tri1[] = {0x00, 0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00};
uint8 code tri2[] = {0x00, 0x00, 0x00, 0x00, 0x11, 0x0A, 0x04, 0x00};
//uint8 code saw1[]={0x00,0x01,0x03,0x05,0x09,0x11,0x00,0x00};
//uint8 code saw2[]={0x00,0x10,0x18,0x14,0x12,0x11,0x00,0x00};
//uint8 code page1_position[] = {0x80, 0x83, 0x87, 0x8C, 0xC0, 0xC5, 0xCB};
//uint8 code page2_position[] = {0x85,0x87};

//判断显示屏是否忙碌
bit Lcd_Check_Busy(void)
{
    bit result;
    E = 0;
    DataPort = 0xFF;
    RS = 0;
    RW = 1;
    E = 1;
    Delay500us();
    result = (bit)(DataPort & 0x80);
    E = 0;
    //DataPort= 0xFF;
    return result;
}

//写命令
void Lcd_Write_Command(uint8 com)
{
    uint8 _cBF = 0;
    while (Lcd_Check_Busy() && _cBF < BF_COUT)
        ; //忙则等待
    {
        _cBF++;
    }
    //while (Lcd_Check_Busy())
    ;       //忙则等待
    RS = 0; //指令输入
    RW = 0; //写操作
    E = 0;
    DataPort = com; //指令赋值
    Delay1ms();     //等待指令稳定
    E = 1;          //使能写入
    Delay1ms();     //等待写指令
    E = 0;          //结束写入
}

//写数据
void Lcd_Write_Data(uint8 dat)
{
    uint8 _cBF = 0;
    while (Lcd_Check_Busy() && _cBF < BF_COUT)
        ; //忙则等待
    {
        _cBF++;
    }
    // while (Lcd_Check_Busy())
    ;       //忙则等待
    RS = 1; //数据输入
    RW = 0; //写操作
    E = 0;
    DataPort = dat; //数据赋值
    Delay1ms();     //等待数据稳定
    E = 1;          //使能写入
    Delay1ms();     //等待写数据
    E = 0;          //结束写入
}

//写字符串
void Lcd_Write_Str(uint8 *s)
{
    while (*s)
    {
        Lcd_Write_Data(*s);
        s++;
    }
}

//输出两位整数
void Lcd_Write_Int(long int i)
{
    if (i >= 1000000)
    {
        Lcd_Write_Data(i / 1000000 + '0');
        if ((i / 100000) % 10 > 0)
        {
            Lcd_Write_Data('.');
            Lcd_Write_Data((i / 100000) % 10 + '0');
        }
        Lcd_Write_Data('M');
    }

    else if (i >= 100000)
    {
        Lcd_Write_Data(i / 100000 + '0');
        Lcd_Write_Data(i / 10000 % 10 + '0');
        Lcd_Write_Data(i / 1000 % 10 + '0');
        Lcd_Write_Data('k');
    }
    else if (i >= 10000)
    {
        Lcd_Write_Data(i / 10000 + '0');
        Lcd_Write_Data(i / 1000 % 10 + '0');
        if ((i / 100) % 10 > 0)
        {
            Lcd_Write_Data('.');
            Lcd_Write_Data((i / 100) % 10 + '0');
        }
        Lcd_Write_Data('k');
    }
    else if (i >= 1000)
    {
        Lcd_Write_Data(i / 1000 + '0');
        if ((i / 100) % 10 > 0)
        {
            Lcd_Write_Data('.');
            Lcd_Write_Data((i / 100) % 10 + '0');
        }
        Lcd_Write_Data('k');
    }
    else if (i >= 100)
    {
        Lcd_Write_Data(i / 100 + '0');
        Lcd_Write_Data(i / 10 % 10 + '0');
        Lcd_Write_Data(i % 10 + '0');
    }
    else if (i >= 10)
    {
        Lcd_Write_Data(i / 10 + '0');
        Lcd_Write_Data(i % 10 + '0');
    }
    else
    {
        //Lcd_Write_Data(' ');
        Lcd_Write_Data(i + '0');
    }
}

//unsigned char code hex_table[16] =
//	{
//		'0', '1', '2', '3', '4', '5', '6', '7',
//		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
////------------------------------------------
//void Lcd_Display_HEX(uint num) //最大显示0xFFFF
//{
//	uint a, b, c, d;
//	a = (num / 256) / 16; //取高字节的高4位
//	b = (num / 256) % 16; //取高字节的低4位
//	c = (num % 256) / 16; //取低字节的高4位
//	d = (num % 256) % 16; //取低字节的低4位

//	Lcd_Write_Data(hex_table[a]);
//	Lcd_Write_Data(hex_table[b]);
//	Lcd_Write_Data(hex_table[c]);
//	Lcd_Write_Data(hex_table[d]);
//}

//void Lcd_Display_INT(uint num) //最大显示65535
//{
//	uint a, b, c, d, e;
//	a = num / 10000;		//万位
//	b = num % 10000 / 1000; //千位
//	c = num % 1000 / 100;   //百位
//	d = num % 100 / 10;		//十位
//	e = num % 10 / 1;		//个位

//	if (a > 0)
//		Lcd_Write_Data(hex_table[a]);
//	if (a > 0 || b > 0)
//		Lcd_Write_Data(hex_table[b]);
//	if (a > 0 || b > 0 || c > 0)
//		Lcd_Write_Data(hex_table[c]);
//	if (a > 0 || b > 0 || c > 0 || d > 0)
//		Lcd_Write_Data(hex_table[d]);
//	Lcd_Write_Data(hex_table[e]);
//}

void Lcd_Init(void)
{
    int num;
    Delay50ms();
    Lcd_Write_Command(LCD_INIT);
    Delay50ms();
    Lcd_Write_Command(LCD_INIT);
    Delay50ms();
    Lcd_Write_Command(LCD_INIT);
    Delay50ms();
    Lcd_Write_Command(LCD_INIT);
    Lcd_Write_Command(LCD_OFF);
    Lcd_Write_Command(LCD_CLEAR);
    Lcd_Write_Command(LCD_MODE);
    Delay50ms();
    Lcd_Write_Command(LCD_NOCURSOR);

    //对CGRAM第一个自定义字符操作，若是第二个则为0x48，其次类推（上面有对顶的关系）
    //方波图标
    Lcd_Write_Command(0x40);
    for (num = 0; num < 8; num++)
    {
        Lcd_Write_Data(rec1[num]);
    }
    Delay5ms();
    //	Lcd_Write_Command(0x48);
    //    for(num=0;num<8;num++)
    //    {
    //        Lcd_Write_Data(rec2[num]);
    //    }
    //Delay5ms();

    //正弦波图标
    Lcd_Write_Command(0x50);
    for (num = 0; num < 8; num++)
    {
        Lcd_Write_Data(sin1[num]);
    }
    Delay5ms();
    Lcd_Write_Command(0x58);
    for (num = 0; num < 8; num++)
    {
        Lcd_Write_Data(sin2[num]);
    }
    Delay5ms();

    ////	//三角波图标
    //    Lcd_Write_Command(0x60);
    //    for (num = 0; num < 8; num++)
    //    {
    //        Lcd_Write_Data(tri1[num]);
    //    }
    //    Delay5ms();
    //    Lcd_Write_Command(0x68);
    //    for (num = 0; num < 8; num++)
    //    {
    //        Lcd_Write_Data(tri2[num]);
    //    }
    //    Delay5ms();

    //	//锯齿波图标
    //    Lcd_Write_Command(0x70);
    //    for (num = 0; num < 8; num++)
    //    {
    //        Lcd_Write_Data(saw1[num]);
    //    }
    //    Delay5ms();
    //
    //	//反锯齿波图标
    //    Lcd_Write_Command(0x78);
    //    for (num = 0; num < 8; num++)
    //    {
    //        Lcd_Write_Data(saw2[num]);
    //    }
}

/*LCD1602自定义字符
0x00：第一个(0x40)                       0x01：第二个(0x48)   
0x02：第三个(0x50)                       0x03：第四个(0x58)
0x04：第五个(0x60)                       0x05：第六个(0x68)   
0x06：第七个(0x70)                       0x07：第八个(0x78)
*/
