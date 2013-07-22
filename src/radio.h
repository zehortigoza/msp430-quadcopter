#ifndef RADIO_H
#define RADIO_H

#define MAX_STRING 100

typedef void (*radio_data_callback)(char *text);

void radio_init(radio_data_callback func);
int radio_send(char *txt);

void radio_tx_int(void);

#endif
