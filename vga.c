#include "vga.h"

// keep track of cursor positions
static uint8_t xPos = 0, yPos = 0;
static uint8_t screenColor = 0;
static uint8_t savedXPos   = 0;

void vga_clear(const uint8_t character)
{
    uint8_t* video = (unsigned char*)VIDEO_MEMORY;

    // clear out each index with our character and attribute byte
    for (uint32_t i = 0; i < COLUMNS * ROWS * 2; i += 2)
    {
        video[i]     = character;
        video[i + 1] = screenColor;
    }

    // reset cursor
    xPos = 0;
    yPos = 0;

    vga_update_cursor(xPos, yPos);
}

void screenScroll()
{
    uint8_t* video = (unsigned char*)VIDEO_MEMORY;
    if (yPos >= ROWS)
    {
        //move columns up
        for (uint32_t i = 1; i < ROWS; i++)
        {
            for (uint32_t j = 0; j < COLUMNS; j++)
            {
                video[(i - 1) * COLUMNS * 2 + j * 2] = video[i * COLUMNS * 2 + j * 2];
            }
        }

        //now clear the last line
        for (uint32_t j = 0; j < COLUMNS; j++)
        {
            video[(ROWS - 1) * COLUMNS * 2 + j * 2] = ' ';
        }

        yPos = ROWS - 1;
    }
}

void vga_putc(const char character)
{
    if (character == '\0')
    {
        return;
    }

    // encounter newline, move to update cursor
    if (character == '\n' || character == '\r')
    {
        xPos = savedXPos;
        yPos++;
        screenScroll();
        vga_update_cursor(xPos, yPos);
        return;
    }

    // if we at end of screen
    if (xPos == COLUMNS)
    {
        xPos = savedXPos;
        yPos++;
        screenScroll();
    }

    // offset to the correct memory address and
    // set the character and the attribute byte
    uint8_t* video = (unsigned char*)VIDEO_MEMORY;
    video += xPos * 2 + yPos * COLUMNS * 2;
    *video++ = (uint8_t)character;
    *video   = screenColor;

    // update the xPos
    xPos = (uint8_t)((int)xPos + 1);

    vga_update_cursor(xPos, yPos);
}

void vga_backspace()
{
	uint8_t x = vga_get_x();
	uint8_t y = vga_get_y();

	//go to previous line
	if(x == 0)
	{
		/*
		uint8_t y = vga_get_y();
		if(y == 0)
		{
			//do nothing if we are at top
		}
		else
		{
			//go to previous last line and find the a character that is not a space
			x = COLUMNS - 1;
			y--;
    			uint8_t* video = (uint8_t*)VIDEO_MEMORY;
	    		video += x * 2 + y * COLUMNS * 2;
			while(x > 0 && *video == ' ')
			{
				x--;
				video -= 2;
			}
			x++;

			vga_go_to_pixel(x, y);
		}
		*/
	}
	//move back a space
	else
	{
		vga_go_to_pixel((uint8_t)((int)x - 1), y);
	}
	
	//get updated values
	x = vga_get_x();
	y = vga_get_y();
	
	//print a space at the current location
    		uint8_t* video = (unsigned char*)VIDEO_MEMORY;
	    video += x * 2 + y * COLUMNS * 2;
	    *video++ = ' ';
	    *video   = screenColor;
}

void vga_puts(char* str)
{
    for (char* c = str; *c != '\0'; c++)
    {
        vga_putc(*c);
    }
}

int vga_printf(const char* str, ...)
{
    va_list list;
    va_start(list, str);

    for (size_t i = 0; i < strlen(str); i++)
    {
        switch (str[i])
        {
        case '%':
        {
            switch (str[i + 1])
            {
            case 'c':
            {
                char c = va_arg(list, char);
                vga_putc(c);
                i++;
                break;
            }
            case 's':
            {
                char* input = va_arg(list, char*);
                vga_puts(input);
                i++;
                break;
            }
            case 'd':
            case 'i':
            {
                int32_t integer = va_arg(list, int32_t);
                char buf[32]    = { 0 };
                itoa(integer, buf, 10);
                vga_puts(buf);
                i++;
                break;
            }
            case 'x':
            case 'X':
            {
                uint32_t integer = va_arg(list, uint32_t);
                char buf[32]     = { 0 };
                itoa_u(integer, buf, 16);
                vga_puts(buf);
                i++;
                break;
            }
            default:
            {
                vga_putc(str[i]);
                i++;
                break;
            }
            }
            break;
        }
        default:
        {
            vga_putc(str[i]);
            break;
        }
        }
    }

    va_end(list);
    return 1;
}

void vga_set_color(const uint8_t color)
{
    screenColor = color;
}

void vga_go_to_pixel(const uint8_t x, const uint8_t y)
{
	//check out of bounds
	if(x >= COLUMNS || y >= ROWS)
	{
		return;
	}

    savedXPos = 0;
    xPos      = x;
    yPos      = y;
    vga_update_cursor(xPos, yPos);
}

uint8_t vga_get_x()
{
    return xPos;
}

uint8_t vga_get_y()
{
    return yPos;
}

void vga_update_cursor(const uint8_t x, const uint8_t y)
{
    uint16_t location = (uint16_t)((uint32_t)COLUMNS * (uint32_t)y + (uint32_t)x);

    //disable_interrupts();
    __asm__ __volatile__("cli");

    //write high byte
    port_write_byte(0x3D4, 14);
    port_write_byte(0x3D5, (uint8_t)(location >> 8));

    //write low byte
    port_write_byte(0x3D4, 15);
    port_write_byte(0x3D5, (uint8_t)(location & 0x00FF));

    //enable_interrupts();
    __asm__ __volatile__("sti");
}
