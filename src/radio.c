#include "main.h"

static radio_data_callback radio_data_func = NULL;

static char tx_buffer[MAX_STRING+1];
static char *tx_ptr = NULL;
static char *tx_next = NULL;

static char rx_buffer[MAX_STRING];
static char *rx_ptr = rx_buffer;

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
    IE2 |= UCA0RXIE + UCA0TXIE;//enable rx interrupt
}

unsigned char radio_send(char *txt)
{
    if (strlen(txt) > MAX_STRING)
        return 0;
    if (tx_ptr)
    {
        if (tx_next)
            return 0;
        tx_next = strdup(txt);
    }

    sprintf(tx_buffer, "%s", txt);

    UCA0TXBUF = tx_buffer[0];
    tx_ptr = tx_buffer + 1;

    return 1;
}

void radio_tx_int(void)
{
    if (*tx_ptr)
    {
        UCA0TXBUF = *tx_ptr;
        tx_ptr++;
    }
    else
    {
        //end of transmition
        if (tx_next)
        {
            sprintf(tx_buffer, "%s", tx_next);
            free(tx_next);
            tx_next = NULL;
            UCA0TXBUF = tx_buffer[0];
            tx_ptr = tx_buffer + 1;
        }
        else
        {
            tx_ptr = NULL;
            IFG2 &= ~UCA0TXIFG;
        }
    }
}

interrupt(USCIAB0RX_VECTOR) rx_int(void)
{
    char c = UCA0RXBUF;
    if (c == '\n' || (rx_ptr == rx_buffer + sizeof(rx_buffer)))
    {
        radio_data_func(rx_buffer);
        rx_ptr = rx_buffer;
        memset(rx_buffer, 0, MAX_STRING);
    }
    else
    {
        *rx_ptr = c;
        rx_ptr++;
    }
}
