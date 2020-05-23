/*
	wave.c
	
    Generate Square and Sine waveform functions

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

    Note:
    2020-05-17 Update
    1. Fix an error in PWM_Config(), may casue wrong output PWM frequency when frequency less than 50Hz.
    1. 修正PWM_Config()中的一处算法错误，会引起小于50Hz的方波输出错误频率。
*/

#include "wave.h"
#include "T_SineTable.h"

bit WAVE_ON = 0;                       //输出波形标志位 1:ON 0:OFF
uint8 PWMCKS_PS = 0;                   //系统时钟分频系数
uint32 PWM_Hz = 100;                   //PWM频率
uint32 PWM_Hz_Pre;                     //记录上一次PWM频率
uint32 PWM_Cycle;                      //PWM周期(最大值为32767)
int8 PWM_Duty = 50;                    //PWM占空比
uint32 PWM_Width;                      //PWM高电平宽度
uint32 SIN_Hz = 100;                   //SIN频率
uint8 Wave_Shape = 1;                  //波形标志 1:方波 2:正弦波
uint8 Wave_Shape_Pre;                  //上次波形标志
uint16 PWM_Index = 0;                  //SPWM查表索引
uint16 T_SinTable_Current[SIN_POINTS]; //根据原始SIN值计算新的SIN表
uint8 Sin_Table_Times = 1;             //SIN倍率
uint32 PWM1_high, PWM1_low;
uint16 n, n_high, n_low;

void Set_PWMCKS_PS(void)
{
    if (PWM_Hz <= (FOSC / 0X7fff)) //0X7fff=32767是15位PWM计数器的最大值，因此FOSC / 0X7fff是主时钟不分频的情况下PWM_Hz的最小值
    {
        PWMCKS_PS = 0x0F;
    }
    else
    {
        PWMCKS_PS = 0x00;
    }
}

void Set_PWM_Cycle(void) //PWM周期，大于50Hz时使用增强型PWM波形发生器直接生成，适用STC15W4K和STC8，小于50Hz使用定时器控制GPIO翻转
{
    if (PWM_Hz < 50)
    {
        PWM_Cycle = FOSC / CKS_50HZ / PWM_Hz; //使用定时器1循环生成低频方波
    }
    else
    {
        PWM_Cycle = (FOSC * 10 / (PWMCKS_PS + 1) / PWM_Hz + 5) / 10 - 1; //使用STC15W4K的高精度PWM生成高频方波
    }
}

void Set_PWM_Width(void) //PWM高电平宽度
{
    PWM_Width = (PWM_Cycle * PWM_Duty * 10 + 5) / 10 / 100;
}

void Set_Sin_Table_Times()
{
    Sin_Table_Times = 1;
    if (SIN_Hz > 6000)
        Sin_Table_Times = 100;
    else if (SIN_Hz > 5000)
        Sin_Table_Times = 50;
    else if (SIN_Hz > 3000)
        Sin_Table_Times = 40;
    else if (SIN_Hz > 2500)
        Sin_Table_Times = 25;
    else if (SIN_Hz > 1000)
        Sin_Table_Times = 20;
    else if (SIN_Hz > 500)
        Sin_Table_Times = 8;
    else if (SIN_Hz > 250)
        Sin_Table_Times = 4;
    else if (SIN_Hz > 100)
        Sin_Table_Times = 2;

    PWM_Hz = SIN_Hz * SIN_POINTS / Sin_Table_Times;
}

void Wave_OFF(void) //关闭波形输出
{
    P_SW2 |= 0x80;  //访问xSFR
    PWMCR &= ~0x80; //关闭PWM模块 （大于50Hz的方波和正弦波）
    PWMCR &= ~0x40; //禁止PWM计数器归零中断 （正弦波）
    PWMIF &= ~CBIF; //清除中断标志
    P_SW2 &= ~0x80; //恢复访问XRAM

    TR1 = 0; //关闭定时器1（小于50Hz的方波）
    TF1 = 0; //清除定时器1中断标志

    //PWM IO状态
    PWM3 = 0;      //设置PWM3 P4.5低电平 方波
    PWM4 = 0;      //设置PWM4 P4.4低电平	正弦波
    P4M1 |= 0x30;  //设置P4.4(PWM4_2),4.5(PWM3_2)为高阻
    P4M0 &= ~0x30; //设置P4.4(PWM4_2),4.5(PWM3_2)为高阻
}

void PWM_Config(void)
{
    if (WAVE_ON)
    {
        //PWM IO状态
        PWM3 = 0;      //设置PWM3 P4.5低电平
        P4M1 &= ~0x20; //设置P4.5为推挽输出
        P4M0 |= 0x20;  //设置P4.5为推挽输出
                       /* 占空比为0时，始终输出低电平
           Output low when duty cycle is 0*/
        if (PWM_Width == 0)
        {
            TR1 = 0;        //关闭定时器1（小于50Hz的方波）
            TF1 = 0;        //清除定时器1中断标志
            PWMCR &= ~0x02; //PWM通道3的端口为GPIO
            PWM3 = 0;       //PWM通道3始终输出低电平
        }
        /* 占空比为100%时，始终输出高电平
           Output high when duty cycle is 100%*/
        else if (PWM_Width == PWM_Cycle)
        {
            TR1 = 0;        //关闭定时器1（小于50Hz的方波）
            TF1 = 0;        //清除定时器1中断标志
            PWMCR &= ~0x02; //PWM通道3的端口为GPIO
            PWM3 = 1;       //PWM通道3始终输出高电平
        }
        /* PWM频率大于等于50时，使用内置增强型PWM输出
           Use enhanced PWM waveform generator when PWM frequency higher than or equal to 50*/
        else if (PWM_Hz >= 50)
        {
            P_SW2 |= 0x80;       //访问xSFR
            PWMCKS = 0x00;       //PWM时钟选择
            PWMCKS |= PWMCKS_PS; //系统时钟分频作为PWM时钟
            PWMC = PWM_Cycle;    //设置PWM周期
            PWMCFG &= ~0x02;     //配置PWM的输出初始电平
            PWM3T1 = 0;          //第一次翻转计数器
            PWM3T2 = PWM_Width;  //第二次翻转计数器
            PWM3CR = 0x08;       //PWM3输出到P4.5
            PWMCR = 0x02;        //使能PWM3输出
            PWMCR &= ~0x40;      //禁止PWM计数器归零中断
            PWMCR |= 0x80;       //使能PWM模块
            P_SW2 &= ~0x80;      //恢复访问XRAM
        }
        /* PWM频率小于50时，使用定时器输出
           Use timer when PWM frequency lower than 50*/
        else
        {
            PWMCR &= ~0x02;                                                      //PWM通道3的端口为GPIO
            PWM3 = 0;                                                            //PWM通道3输出低电平
            PWM1_high = PWM_Width;                                               //高电平持续总时间
            PWM1_low = PWM_Cycle - PWM_Width;                                    //低电平持续总时间
            n_high = PWM1_high / 65536;                                          //高电平超过定时器溢出的次数
            n_low = PWM1_low / 65536;                                            //低电平超过定时器溢出的次数
            PWM1_high = 65535 - PWM1_high % 65536 + FOSC * 2 / 10000 / CKS_50HZ; //定时器初值，并修正为判断PWM电平变化延时的200us
            PWM1_low = 65535 - PWM1_low % 65536 + FOSC * 2 / 10000 / CKS_50HZ;   //定时器初值，并修正为判断PWM电平变化延时的200us
            //下面的算法之前写错了，会引起输出频率错误，已修正
            if (PWM1_high > 65535) //修正后的定时器初值大于65535则再次修正
            {
                n_high--;
                PWM1_high -= 65535;
            }
            if (PWM1_low > 65535) //修正后的定时器初值大于65535则再次修正
            {
                n_low--;
                PWM1_low -= 65535;
            }
            n = n_low;
            TH1 = (uint8)(PWM1_low >> 8); //如果是输出低电平，则装载低电平时间。
            TL1 = (uint8)PWM1_low;
            TR1 = 1; //定时器1开始运行
        }
    }
    else
    {
        Wave_OFF();
    }
}

void Sin_Wave_Config(void)
{
    int i;
    float Sin_Cycle_times; //SPWM_interrupt中断函数运行需要时间，SPWM频率120K是STC15W4K32S4的上限，再高就来不及通过SPWM_interrupt调节占空比了

    //T_SinTable的数值是根据SIN_TABLE_PWM_HZ计算得到最大周期，周期为FOSC/SIN_TABLE_PWM_HZ，24MHz主时钟150kHz对应周期160
    //根据当前的PWM_Hz重新计算正弦表，使得正弦波幅尽量大
    Sin_Cycle_times = SIN_TABLE_PWM_HZ * 1.0 / PWM_Hz; //当前PWM_Hz相对SIN_TABLE_PWM_HZ的倍率
    for (i = 0; i < SIN_POINTS; i += Sin_Table_Times)  //重新计算正弦表，并增加偏移量SIN_OFFSET
    {
        T_SinTable_Current[i] = T_SinTable[i] * Sin_Cycle_times + SIN_OFFSET;
    }

    if (WAVE_ON)
    {
        //PWM IO状态
        PWM4 = 0;      //设置PWM4 P4.4低电平
        P4M1 &= ~0x10; //设置P4.4为推挽输出
        P4M0 |= 0x10;  //设置P4.4为推挽输出

        P_SW2 |= 0x80;       //访问xSFR
        PWMCR &= ~0x80;      //关闭PWM模块
        PWMCR &= ~0x40;      //禁止PWM计数器归零中断
        PWMIF &= ~CBIF;      //清除中断标志
        PWMCKS = 0x00;       //PWM时钟选择
        PWMCKS |= PWMCKS_PS; //系统时钟分频作为PWM时钟
        PWMC = PWM_Cycle;    //设置PWM周期
        PWM_Index = 0;
        PWM4T1 = 0;                             //第一次翻转计数器
        PWM4T2 = T_SinTable_Current[PWM_Index]; //第二次翻转计数器
        PWM_Index += Sin_Table_Times;
        PWMCFG &= ~0x04; //配置PWM4的输出初始电平
        PWM4CR = 0x08;   //PWM4输出到P4.4，无中断
        PWMCR |= 0x04;   //使能PWM4输出
        PWMCR |= 0x40;   //允许PWM计数器归零中断
        PWMCR |= 0x80;   //使能PWM模块
        P_SW2 &= ~0x80;  //恢复访问XRAM
    }
    else
    {
        Wave_OFF();
    }
}

/* 
   */
void Set_Wave_Shape(void)
{
    if(Wave_Shape==1) //方波
        PWM_Config();
    else if (Wave_Shape == 2) //正弦波
        Sin_Wave_Config();
}

/* 用来生成正弦波的SPWM中断
   SPWM Interrupt for generating the sine waveform*/
void SPWM_interrupt(void) interrupt SPWM_VECTOR
{
    PWMIF &= ~CBIF; //清除中断标志
    _push_(P_SW2);  //保存SW2设置
    P_SW2 |= 0x80;  //访问XFR
    PWM4T2 = T_SinTable_Current[PWM_Index];
    if ((PWM_Index += Sin_Table_Times) >= SIN_POINTS)
        PWM_Index = 0;
    _pop_(P_SW2); //恢复SW2设置
}

/* 50Hz以下PWM使用Timer1中断产生
   Generate PWM below 50Hz by Timer1 Interrupt*/
void TIMER1_interrupt(void) interrupt TIMER_1
{
    TR1 = 0;
    if (n-- == 0)
    {
        PWM3 = !PWM3;
        Delay200us(); //延时等待PWM3电平变化，200us 24MHz对应4800周期，在计时器初值扣除
        if (PWM3)
        {
            n = n_high;
            TH1 = (uint8)(PWM1_high >> 8); //如果是输出高电平，则装载高电平时间。
            TL1 = (uint8)PWM1_high;
        }
        else
        {
            n = n_low;
            TH1 = (uint8)(PWM1_low >> 8); //如果是输出低电平，则装载低电平时间。
            TL1 = (uint8)PWM1_low;
        }
    }
    else
    {
        TH1 = 0x00;
        TL1 = 0x00;
    }
    TF1 = 0;
    TR1 = 1;
}
