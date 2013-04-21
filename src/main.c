#include "main.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;//Stop watchdog timer

    DCOCTL = CALDCO_8MHZ;//MCLK = 8mhz
    BCSCTL1 = CALBC1_8MHZ;
    BCSCTL2 |= DIVS1+DIVS0;//Divider SMCLK per 8, SMLCK = 1Mhz

    agent_init();

    _BIS_SR(LPM0_bits + GIE);//enter in lpm0 mode = cpu and mclk off, aclk, smclk and interruption on

    return 0;
}
