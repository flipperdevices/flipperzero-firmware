#ifndef TIMER_H
#define TIMER_H
void timer_set_tac(unsigned char);
void timer_cycle(unsigned int cycles);
unsigned char timer_get_div(void);
unsigned char timer_get_counter(void);
unsigned char timer_get_modulo(void);
unsigned char timer_get_tac(void);
void timer_set_div(unsigned char);
void timer_set_counter(unsigned char);
void timer_set_modulo(unsigned char);
#endif