#include "io.h"

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   24

/* Save the X position. */
static int xpos;
/* Save the Y position. */
static int ypos;

static char *fb = (char *)0x000B8000;

void outb(unsigned short port, unsigned char data);
void fb_move_cursor(unsigned short pos);
static void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
static void itoa (char *buf, int base, int d);
static void putchar (int c);
void printf (const char *format, ...);


void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 *
 *  Note: i should be 0, 2, 4, 6, ...., 3998, because 80 * 25 * 2 = 4000
 */
static void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}


/* https://www.gnu.org/software/grub/manual/multiboot/multiboot.html */
static void putchar (int c)
{
    if (c == '\n' || c == '\r') {
newline:
        xpos = 0;
        ypos++;
        if (ypos >= LINES)
            ypos = 0;
        return;
    }

    fb_write_cell((xpos + ypos * COLUMNS) * 2, c, FB_GREEN, FB_LIGHT_BROWN);

    xpos++;
    if (xpos >= COLUMNS)
        goto newline;
}

/* https://www.gnu.org/software/grub/manual/multiboot/multiboot.html */
static void itoa (char *buf, int base, int d)
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    /* If %d is specified and D is minus, put `-' in the head. */
    if (base == 'd' && d < 0) {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0. */
    do {
        int remainder = ud % divisor;

        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
    while (ud /= divisor);

    /* Terminate BUF. */
    *p = 0;

    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

/* https://www.gnu.org/software/grub/manual/multiboot/multiboot.html */
void printf (const char *format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[20];

    arg++;

    while ((c = *format++) != 0) {
        if (c != '%')
            putchar (c);
        else {
            char *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0') {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9') {
                pad = c - '0';
                c = *format++;
            }

            switch (c) {
                case 'd':
                case 'u':
                case 'x':
                    itoa (buf, c, *((int *) arg++));
                    p = buf;
                    goto string;
                    break;

                case 's':
                    p = *arg++;
                    if (! p)
                        p = "(null)";

string:
                    for (p2 = p; *p2; p2++);
                    for (; p2 < p + pad; p2++)
                        putchar (pad0 ? '0' : ' ');
                    while (*p)
                        putchar (*p++);
                    break;

                default:
                    putchar (*((int *) arg++));
                    break;
            }
        }
    }
}

void cls (void)
{
    int i;

    for (i = 0; i < COLUMNS * LINES * 2; i++)
        *(fb + i) = 0;

    xpos = 0;
    ypos = 0;
}
