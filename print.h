#ifndef PRINT_H
#define PRINT_H

void print_char(char);
void print(char *);
void cls();
void print_dec(unsigned int);
void print_hex(unsigned int);
void print_bin(unsigned int);
void printf(char *, ...);
unsigned char get_cursor_x();
unsigned char get_cursor_y();
void set_cursor(unsigned char, unsigned char);
void init_cursor();

#endif
