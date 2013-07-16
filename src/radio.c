#include "main.h"

static radio_data_callback radio_data_func = NULL;

static char tx_buffer[MAX_STRING];
static unsigned char tx_buffer_index = 0;

static char rx_buffer[MAX_STRING];
static unsigned char rx_buffer_index = 0;//TODO change to char pointer

void radio_init(radio_data_callback func)
{
    radio_data_func = func;

    memset(rx_buffer, 0, MAX_STRING);
    memset(tx_buffer, 0, MAX_STRING);

    P1SEL = BIT1 + BIT2;// P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;

    UCA0CTL1 |= UCSSEL_2;//smclk
    UCA0BR0 = 104;//9600
    UCA0BR1 = 0;//9600
    UCA0MCTL = UCBRS0;//modulation
    UCA0CTL1 &= ~UCSWRST;//initialize USCI state machine
    IE2 |= UCA0RXIE;//enable rx interrupt
}

char *radio_tx_buffer_get(void)
{
    return tx_buffer;
}

int radio_send(void)
{
    if (tx_buffer_index)
        return 0;

    IE2 |= UCA0TXIE;
    UCA0TXBUF = tx_buffer[tx_buffer_index];
    tx_buffer_index++;

    return 1;
}

void radio_tx_int(void)
{
    if (tx_buffer[tx_buffer_index])
    {
        UCA0TXBUF = tx_buffer[tx_buffer_index];
        tx_buffer_index++;
    }
    else
    {
        //end of transmition
        IE2 &= ~UCA0TXIE;
        memset(tx_buffer, 0, MAX_STRING);
        tx_buffer_index = 0;
    }
}

interrupt(USCIAB0RX_VECTOR) rx_int(void)
{
    char c = UCA0RXBUF;
    if (c == '\n')
    {
        radio_data_func(rx_buffer);
        rx_buffer_index = 0;
        memset(rx_buffer, 0, MAX_STRING);
    }
    else
    {
        rx_buffer[rx_buffer_index] = c;
        rx_buffer_index++;
    }
}
