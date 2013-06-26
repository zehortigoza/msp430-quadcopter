#include "main.h"

interrupt(USCIAB0TX_VECTOR) usci_ab_tx_int(void)
{
    if (IFG2 & UCB0TXIFG || IFG2 & UCB0RXIFG)
        i2c_rx_tx_int();
    if (IFG2 & UCB0TXIFG)
        radio_tx_int();
}
