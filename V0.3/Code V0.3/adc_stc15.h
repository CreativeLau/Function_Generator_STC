#ifndef ADC_STC15_H
#define ADC_STC15_H

#define STC15
#ifdef STC15

//#include "stc_header.h"
#include "delay.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif	

#ifndef uint32
#define uint32 unsigned long int
#endif

//Bandgap电压在RAM和ROM中的存放地址，单位mV，RAM中的数据有可能被冲掉，所以推荐使用ROM中的数据，RAM中的数据可以作为备选
//WORD idata Vbg_RAM _at_ 0xef; //对于只有256字节RAM的MCU存放地址为0EFH
//WORD idata Vbg_RAM _at_ 0x6f; //对于只有128字节RAM的MCU存放地址为06FH
//注意:需要在下载代码时选择"在ID号前添?重要测试参数"选项,才可在程序ROM中获取此参数
//WORD code Vbg_ROM _at_ 0x03f7; //1K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x07f7; //2K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x0bf7; //3K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x0ff7; //4K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x13f7; //5K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x1ff7; //8K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x27f7; //10K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x2ff7; //12K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x3ff7; //16K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x4ff7; //20K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x5ff7; //24K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x6ff7; //28K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x7ff7; //32K程序空间的MCU
//WORD code Vbg_ROM _at_ 0x9ff7; //40K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xbff7; //48K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xcff7; //52K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xdff7; //56K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xeff7; //60K程序空间的MCU
//WORD code Vbg_ROM _at_ 0xfdf7; //64K程序空间的MCU
#define BGV_ADR 0x7ff7
//-----------------------------------------
sfr ADC_CONTR = 0xBC; //ADC控制寄存器
sfr ADC_RES = 0xBD; //ADC高8位结果
sfr ADC_RESL = 0xBE; //ADC低2位结果
sfr P1ASF = 0x9D; //P1口第2功能控制寄存器
sfr CLK_DIV =   0x97;   //0000,0000 时钟分频控制寄存器

#define ADC_POWER 0x80   	//ADC电源控制位
#define ADC_FLAG 0x10		//ADC完成标志
#define ADC_START 0x08   	//ADC起始控制位
#define ADC_SPEEDLL 0x00 	//540个时钟
#define ADC_SPEEDL 0x20  	//360个时钟
#define ADC_SPEEDH 0x40  	//180个时钟
#define ADC_SPEEDHH 0x60 	//90个时钟		
#define ADRJ 0x20 			//当ADRJ=1时， 10位A/D转换结果的高2位存放在ADC_RES的低2位中，低8位存放在ADC_RESL中
							//当ADRJ=0时， 10位A/D转换结果的高8位存放在ADC_RES中，低2位存放在ADC_RESL的低2位中
#define ADC_CHS_10 0x00 	//将P1.0的模拟值输入ADC模拟通道
#define ADC_CHS_11 0x01 	//将P1.1的模拟值输入ADC模拟通道
#define ADC_CHS_12 0x02 	//将P1.2的模拟值输入ADC模拟通道
#define ADC_CHS_13 0x03 	//将P1.3的模拟值输入ADC模拟通道
#define ADC_CHS_14 0x04 	//将P1.4的模拟值输入ADC模拟通道
#define ADC_CHS_15 0x05 	//将P1.5的模拟值输入ADC模拟通道
#define ADC_CHS_16 0x06 	//将P1.6的模拟值输入ADC模拟通道
#define ADC_CHS_17 0x07 	//将P1.7的模拟值输入ADC模拟通道
#define ADC_CHS_BG 0x09 	//将内部BandGap定义为第九通道
#define P1ASF_0 0x01		//将P1.0设置为AD模拟输入模式
#define P1ASF_1 0x02		//将P1.1设置为AD模拟输入模式
#define P1ASF_2 0x04		//将P1.2设置为AD模拟输入模式
#define P1ASF_3 0x08		//将P1.3设置为AD模拟输入模式
#define P1ASF_4 0x10		//将P1.4设置为AD模拟输入模式
#define P1ASF_5 0x20		//将P1.5设置为AD模拟输入模式
#define P1ASF_6 0x40		//将P1.6设置为AD模拟输入模式
#define P1ASF_7 0x80		//将P1.7设置为AD模拟输入模式
#define P1ASF_BG 0x00		//读内部BandGap时，关闭所有ADC通道

uint16 Get_VCC_Voltage();

#endif
#endif