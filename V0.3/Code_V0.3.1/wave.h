#ifndef WAVE_H
#define WAVE_H

#include <reg51.h>
#include <intrins.h>
#include "config_stc.h"
#include "delay.h"

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef int8
#define int8 char
#endif

#ifndef uint16
#define uint16 unsigned int
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

#define FOSC 24000000UL                           //主时钟
#define SPWM_VECTOR 22                             //PWM中断序号
#define TIMER_1 3                                 //定时器1中断序号
#define CBIF 0x40                                 //PWM计数器归零中断标志
#define SIN_TABLE_PWM_HZ 150000                   //T_SinTable计算使用的PWM频率，用FOSC/SIN_TABLE_PWM_HZ求出PWM最大宽度
#define SIN_POINTS 1000                           //T_SinTable点数
#define SIN_OFFSET 1                              //SIN值偏移量，用于修正第一次翻转和第二次翻转都为0的情况，本应该翻转两次，这样只翻转了一次，造成电平反向
#define WAVE_NUM 2                                //波形选项的数量，暂时只做了方波和正弦波，设置为2，当Wave_Shape=0时，跳转到显示VCC电压
#define PWM_MAX_DUTY 99                           //PWM最大占空比
#define PWM_MIN_DUTY 1                            //PWM最小占空比
#define PWM_MIN_WIDTH 200                         //PWM最小宽度持续时间 ns
#define PWM_MAX_HZ 2000000                        //PWM最大频率2MHz
#define PWM_MIN_HZ 1                              //PWM最小频率1Hz
#define SIN_MAX_HZ 10000                          //SIN最大频率10kHz
#define SIN_MIN_HZ 1                              //SIN最小频率1Hz
#define CKS_50HZ 12                               //小于50Hz的时钟分频
#define PWMC (*(uint16 volatile xdata *)0xfff0)   //PWM计数器
#define PWMCKS (*(uint8 volatile xdata *)0xfff2)  //PWM时钟选择位
#define PWM3T1 (*(uint16 volatile xdata *)0xff10) //PWM3T1计数器
#define PWM3T2 (*(uint16 volatile xdata *)0xff12) //PWM3T2计数器
#define PWM3CR (*(uint8 volatile xdata *)0xff14)  //PWM3控制位
#define PWM4T1 (*(uint16 volatile xdata *)0xFF20) //PWM4T1计数器
#define PWM4T2 (*(uint16 volatile xdata *)0xFF22) //PWM4T2计数器
#define PWM4CR (*(uint8 volatile xdata *)0xFF24)  //PWM4控制位

extern uint8 PWMCKS_PS;       //系统时钟分频系数
extern uint32 PWM_Hz;         //PWM频率
extern uint32 PWM_Hz_Pre;     //记录上一次PWM频率
extern int8 PWM_Duty;         //PWM占空比
extern uint32 SIN_Hz;         //SIN频率
extern uint8 Wave_Shape;      //波形标志 1:方波 2:正弦波
extern uint8 Wave_Shape_Pre;  //上次波形标志
extern uint8 Sin_Table_Times; //SIN倍率
extern bit WAVE_ON;           //输出波形标志位 1:ON 2:OFF

void Set_Wave_Shape();
void Wave_OFF();
#endif