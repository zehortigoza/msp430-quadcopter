#include "main.h"

interrupt(USCIAB0TX_VECTOR) tx_int(void)
{
    if(IFG2 && UCA0TXIE)
        radio_tx_int();
    else if(IFG2 && UCB0TXIFG || IFG2 && UCB0RXIFG)
        sensor_tx_int();
}
