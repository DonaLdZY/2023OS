#include "stdio.h"
#include "os_type.h"
#include "asm_utils.h"
#include "os_modules.h"
#include "stdarg.h"
#include "stdlib.h"

STDIO::STDIO()
{
    initialize();
}

void STDIO::initialize()
{
    screen = (uint8 *)0xb8000;
}

void STDIO::print(uint x, uint y, uint8 c, uint8 color)
{

    if (x >= 25 || y >= 80)
    {
        return;
    }

    uint pos = x * 80 + y;
    screen[2 * pos] = c;
    screen[2 * pos + 1] = color;
}

void STDIO::print(uint8 c, uint8 color)
{
    uint cursor = getCursor();
    screen[2 * cursor] = c;
    screen[2 * cursor + 1] = color;
    cursor++;
    if (cursor == 25 * 80)
    {
        rollUp();
        cursor = 24 * 80;
    }
    moveCursor(cursor);
}

void STDIO::print(uint8 c)
{
    print(c, 0x07);
}

void STDIO::moveCursor(uint position)
{
    if (position >= 80 * 25)
    {
        return;
    }

    uint8 temp;

    // 处理高8位
    temp = (position >> 8) & 0xff;
    asm_out_port(0x3d4, 0x0e);
    asm_out_port(0x3d5, temp);

    // 处理低8位
    temp = position & 0xff;
    asm_out_port(0x3d4, 0x0f);
    asm_out_port(0x3d5, temp);
}

uint STDIO::getCursor()
{
    uint pos;
    uint8 temp;

    pos = 0;
    temp = 0;
    // 处理高8位
    asm_out_port(0x3d4, 0x0e);
    asm_in_port(0x3d5, &temp);
    pos = ((uint)temp) << 8;

    // 处理低8位
    asm_out_port(0x3d4, 0x0f);
    asm_in_port(0x3d5, &temp);
    pos = pos | ((uint)temp);

    return pos;
}

void STDIO::moveCursor(uint x, uint y)
{
    if (x >= 25 || y >= 80)
    {
        return;
    }

    moveCursor(x * 80 + y);
}

void STDIO::rollUp()
{
    uint length;
    length = 25 * 80;
    for (uint i = 80; i < length; ++i)
    {
        screen[2 * (i - 80)] = screen[2 * i];
        screen[2 * (i - 80) + 1] = screen[2 * i + 1];
    }

    for (uint i = 24 * 80; i < length; ++i)
    {
        screen[2 * i] = ' ';
        screen[2 * i + 1] = 0x07;
    }
}

int STDIO::print(const char *const str)
{
    int i = 0;

    for (i = 0; str[i]; ++i)
    {
        switch (str[i])
        {
        case '\n':
            uint row;
            row = getCursor() / 80;
            if (row == 24)
            {
                rollUp();
            }
            else
            {
                ++row;
            }
            moveCursor(row * 80);
            break;

        default:
            print(str[i]);
            break;
        }
    }

    return i;
}

int printf_add_to_buffer(char *buffer, char c, int &idx, const int BUF_LEN)
{
    int counter = 0;

    buffer[idx] = c;
    ++idx;

    if (idx == BUF_LEN)
    {
        buffer[idx] = '\0';
        counter = stdio.print(buffer);
        idx = 0;
    }

    return counter;
}

int printf(const char *const fmt, ...)
{
    const int BUF_LEN = 32;

    char buffer[BUF_LEN + 1];
    char number[33];

    int idx, counter;
    va_list ap;

    va_start(ap, fmt);
    idx = 0;
    counter = 0;

    for (int i = 0; fmt[i]; ++i)
    {
    	int t=0,t2=0,t3=0;
    	char *strs=nullptr;
        if (fmt[i] != '%')
        {
            counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
        }
        else
        {
            i++;
            while (fmt[i]>='0' && fmt[i]<='9'){
            	t=(t<<3)+(t<<1)+fmt[i]-'0';
            	i++;
            }
            
            if (fmt[i] == '\0')
            {
                break;
            }
	
            switch (fmt[i])
            {
            case '%':
                counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
                break;

            case 'c':
            	for (;t>1;t--) 
            		counter += printf_add_to_buffer(buffer,' ', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, va_arg(ap, char), idx, BUF_LEN);
                break;

            case 's':   
                strs=va_arg(ap,char *);
                for(;strs[t2];t2++){}
                for (;t>t2;t--){
                	counter += printf_add_to_buffer(buffer,' ', idx, BUF_LEN);
                }
            	buffer[idx] = '\0';
                idx = 0;
                counter += stdio.print(buffer);
                counter += stdio.print(strs);
                break;

            case 'd':
            case 'x':
                int temp = va_arg(ap, int);
		
                if (temp < 0 && fmt[i] == 'd')
                {
                    
                    temp = -temp;
                    t2=t3=1;
                }
                itos(number, temp, (fmt[i] == 'd' ? 10 : 16));
                for(;number[t2];t2++){}
                for (;t>t2;t--){
                	counter += printf_add_to_buffer(buffer,' ', idx, BUF_LEN);
                }
		if (t3)
                {
                    counter += printf_add_to_buffer(buffer, '-', idx, BUF_LEN);
                }
                for (int j = 0; number[j]; ++j)
                {
                    counter += printf_add_to_buffer(buffer, number[j], idx, BUF_LEN);
                }
                break;
            }
            
        }
    }

    buffer[idx] = '\0';
    counter += stdio.print(buffer);

    return counter;
}
