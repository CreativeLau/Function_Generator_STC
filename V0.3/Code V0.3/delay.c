#include "delay.h"

void Delay500us() //@24.000MHz
{
    unsigned char i, j;

    i = 12;
    j = 169;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay1ms() //@24.000MHz
{
    unsigned char i, j;

    i = 24;
    j = 85;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay5ms() //@24.000MHz
{
    unsigned char i, j;

    i = 117;
    j = 184;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void Delay50ms() //@24.000MHz
{
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 5;
    j = 144;
    k = 71;
    do
    {
        do
        {
            while (--k)
                ;
        } while (--j);
    } while (--i);
}

bit Delay500ms_long_click() //@24.000MHz
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
    i = 46;
    j = 153;
    k = 245;
    do
    {
        do
        {
            if (EC11_KEY)
                return 0;
            while (--k)
                ;
        } while (--j);
    } while (--i);
    return 1;
}

bit Delay200ms_double_click() //@24.000MHz
{
    unsigned char i, j, k;
    Delay50ms();
    _nop_();
    _nop_();
    i = 19;
    j = 62;
    k = 43;
    do
    {
        do
        {
            if (!EC11_KEY)
                return 1;
            while (--k)
                ;
        } while (--j);
    } while (--i);
    return 0;
}

void Delay200us() //@24.000MHz
{
    unsigned char i, j;

    i = 5;
    j = 168;
    do
    {
        while (--j)
            ;
    } while (--i);
}
