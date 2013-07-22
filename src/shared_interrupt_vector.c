#include "main.h"

interrupt(USCIAB0TX_VECTOR) usci_ab_tx_int(void)
{
    /**
     * something was setting the UCB flag even without config UCB, because of
     * this i2c is only called if there is no interruption in UCA
     */
    if (IFG2 & UCA0TXIFG)
        radio_tx_int();
    else
        i2c_rx_tx_int();
}
