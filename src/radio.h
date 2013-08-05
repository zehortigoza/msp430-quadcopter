#ifndef RADIO_H
#define RADIO_H

#define MAX_STRING 50

typedef void (*radio_data_callback)(char *text);

void radio_init(radio_data_callback func);
unsigned char radio_send(char *txt);

void radio_tx_int(void);

#endif
