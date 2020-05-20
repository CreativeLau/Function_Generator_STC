/*
	adc_stc15.c
	
    Get voltage infor of VCC

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

#include "adc_stc15.h"

#ifdef STC15

//在主程序入口将ADC模拟输入的IO设置为高阻状态PnM1.x=1,PnM0.x=0
//说明:
// ADC的第9通道(STC15系列第9通道，STC8系列第16通道)是用来测试内部BandGap参考电压的,由于内部BandGap参考电
//压很稳定,不会随芯片的工作电压的改变而变化,所以可以通过两次测量和一次计算
//便可得到外部的精确电压.公式如下:
//
//STC15系列10位ADC
//ADCbg / Vbg = 1023 / VCC
//ADCx / Vx = 1023 / VCC
//
//STC8系列12位ADC
//ADCbg / Vbg = 4095 / VCC
//ADCx / Vx = 4095 / VCC
//
//由于两次测量的时间间隔很?,VCC的电压在此期间的波动可忽略不计
//从而可推出 ADCbg / Vbg = ADCx / Vx
//进一步得出 Vx = Vbg * ADCx / ADCbg
//其中:ADCbg为Bandgap电压的ADC测量值
// Vbg为实际Bandgap的电压值,在单片机进行CP测试时记录的参数,单位为毫伏(mV)
// ADCx为外部输入电压的ADC测量值
// Vx外部输入电压的实际电压值,单位为毫伏(mV)
//
//具体的测试方法:首先将P1ASF初始化为0,即关闭所有P1口的模拟功能
//然后通过正常的ADC转换的方法读取第0通道的值,即可通过ADC的第9通道读取当前
//内部BandGap参考电压值ADCbg,然后测量有外部电压输入的ADC通道,测量出
//外部输入电压的ADC测量值ADCx,接下来从RAM区或者ROM区读取实际Bandgap的电压值Vbg,
//最后通过公式Vx = Vbg * ADCx / ADCbg,即可计算出外部输入电压的实际电压值Vx
//-----------------------------------------

//BGV内部参考电压
int *BGV;
//int *Vbg_RAM;

void ADCInit()
{
    CLK_DIV &= ~ADRJ;                    //当ADRJ=0时， 10位A/D转换结果的高8位存放在ADC_RES中，低2位存放在ADC_RESL的低2位中
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL; //ADC上电，设定转换速度
                                         //	ADC_CONTR|=ADC_POWER;			官方不建议用与和或的方式给ADC_CONTR赋值
                                         //	ADC_CONTR|=ADC_SPEEDLL;
    Delay5ms();                          //ADC上电并延时
}

int ADCRead(uint8 chx)
{
    int res;
    ADC_RES = 0; //清除结果寄存器
    ADC_RESL = 0;
    switch (chx)
    {
    case ADC_CHS_BG:
        P1ASF = P1ASF_BG;
        chx = P1ASF_BG;
        break;
    case ADC_CHS_10:
        P1ASF = P1ASF_0;
        break;
    case ADC_CHS_11:
        P1ASF = P1ASF_1;
        break;
    case ADC_CHS_12:
        P1ASF = P1ASF_2;
        break;
    case ADC_CHS_13:
        P1ASF = P1ASF_3;
        break;
    case ADC_CHS_14:
        P1ASF = P1ASF_4;
        break;
    case ADC_CHS_15:
        P1ASF = P1ASF_5;
        break;
    case ADC_CHS_16:
        P1ASF = P1ASF_6;
        break;
    case ADC_CHS_17:
        P1ASF = P1ASF_7;
        break;
    }
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | chx;
    //ADC_CONTR|=ADC_START;		//官方不建议用与和或的方式给ADC_CONTR赋值
    //ADC_CONTR&=~0x07;
    _nop_(); //等待4个NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG))
        ;                            //等待ADC转换完成
    ADC_CONTR &= ~ADC_FLAG;          //清除ADC标志
    res = (ADC_RES << 2) | ADC_RESL; //读取 ADC 结果
    return res;
}

uint16 Get_VCC_Voltage()
{
    uint32 ADCbg;
    uint16 VCC;
    int i;
    BGV = (int code *)BGV_ADR; //从ROM中取出BandGap电压值，单位mV，用STC-ISP下载程序时需选中“在程序结束处添加重要参数”
    //Vbg_RAM = (int idata *)0xef; 	//从RAM中取出BandGap电压值，单位mV
    ADCInit(); //初次打开内部A/D转换模拟电源，需适当延时，等内部模拟电源稳定后，再启动A/D转换

    //第一步:通过ADC的第9通道读取Bandgap电压的ADC测量值
    ADCRead(ADC_CHS_BG);
    ADCRead(ADC_CHS_BG); //前两个数据丢弃
    ADCbg = 0;
    for (i = 0; i < 512; i++)
    {
        ADCbg += ADCRead(ADC_CHS_BG); //读取 8 次数据
    }
    ADCbg >>= 9; //取平均值
    VCC = *BGV * 1023UL / ADCbg;
    return VCC;
}

#endif