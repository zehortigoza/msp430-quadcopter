#ifndef MOTORS_H
#define MOTORS_H

int motors_init(void);
void motors_velocity_set(unsigned char fr, unsigned char fl, unsigned char br, unsigned char bl);
void motors_velocity_get(unsigned char *fr, unsigned char *fl, unsigned char *br, unsigned char *bl);

#endif
