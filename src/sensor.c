#include "main.h"

#define MPU6050_ADDR (0x0068)//ADO = low

static unsigned char mode;
#define READ_VALUE 1
#define WRITE_VALUE 1 << 1
#define TX_MODE 1 << 2
#define RX_MODE 1 << 3

static char buffer[50];
static int buffer_index;

//USCI_B and I2C The maximum bus speed is 400 kHz

/*
 * which is 7 bits lon
 * the address of the one of the devices should be b1101000 (pin AD0 is logic low)
 * and the address of the other should be b1101001 (pin AD0 is logic high).
 */

//config to some hz, fifo and int

//when int happen
//get raw and send to agent

/*
 * The USCIAB0TX_VECTOR handles only interrupt flags that indicate if a character has been transmitted or received.
 * The USCIAB0RX_VECTOR handles only Start and stop conidtion flags (UCSTPIFG and UCSTTIFG).
 * All in all TX and RX are both handled in the USCIAB0TX interrupt, while I2C state changes are handled in the USCIAB0RX interrupt.
 */

void sensor_init()
{
    P1SEL |= BIT6 + BIT7;//Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;//Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;//Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;//I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;//Use SMCLK, keep SW reset
    UCB0BR0 = 8;// SMCLK/8 = 125kHz
    UCB0BR1 = 0;
    UCB0I2CSA = MPU6050_ADDR;//Set slave address
    UCB0CTL1 &= ~UCSWRST;//Clear SW reset, resume operation
    //IE2 |= UCB0RXIE + UCB0TXIE;// Enable RX/TX interrupt
    snprintf(send_buffer, sizeof(send_buffer), "");
    snprintf(receive_buffer, sizeof(receive_buffer), "");
}

void sensor_transmit()
{
    while(mode == RX_MODE);
    while (UCB0CTL1 & UCTXSTP);//Ensure stop condition got sent
    buffer_index = 0;
    IE2 |= UCB0TXIE;//Enable TX interrupt
    UCB0CTL1 |= UCTR + UCTXSTT;//I2C TX, start condition
}

void sensor_read(unsigned char reg)
{
    while (!mode);
    mode = READ_VALUE + TX_MODE;

    buffer_index = 0;
    snprintf(buffer, sizeof(buffer), "%c", reg);//TODO check if ok

    IE2 |= UCB0TXIE;//Enable TX interrupt
    UCB0CTL1 |= UCTR + UCTXSTT;//I2C TX, start condition
}

void sensor_tx_int(void)
{
    if (mode & TX_MODE)
    {
        if (send_buffer[buffer_index])
        {
            UCB0TXBUF = send_buffer[buffer_index];
            buffer_index++;
        }
        else
        {

        }
    }
    else
    {

    }
}
