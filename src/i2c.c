#include "main.h"

#define I2C_BUFFER_SIZE 32

#define BUS_BUSY BIT0
#define WRITE_MODE BIT1
#define REGISTER_SENT BIT2
#define CONNECTION_RESTARDED BIT3
#define BUS_INITIALIZED BIT4

/**
 * bit0 = busy
 * bit1 = reading(0) or writing(1) a register?
 * bit2 = register was transmitted?
 * bit3 = restart send *only used when reading a register
 * bit4 = bus initialized
 */
static char state = 0;
static i2c_data_read_callback data_read_func;
static i2c_data_write_callback data_write_func;
static unsigned char send_register = 0;
static unsigned char buffer[I2C_BUFFER_SIZE];
static unsigned char *ptr_data;
static int data_size = 0;

short i2c_bus_init(unsigned char address)
{
    if (state)
        return 0;
    P1SEL |= BIT6 + BIT7;//Assign I2C pins to USCI_B0
    P1SEL2 |= BIT6 + BIT7;//Assign I2C pins to USCI_B0

    UCB0CTL1 |= UCSWRST;//USCI logic held in reset state
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;// I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;//Use SMCLK, keep SW reset
    UCB0BR0 = 4;//fSCL = SMCLK/4 = 250kHz
    UCB0BR1 = 0;
    UCB0I2CSA = address;//slave address
    UCB0CTL1 &= ~UCSWRST;//release USCI
    IE2 |= UCB0TXIE + UCB0RXIE;
    state = BUS_INITIALIZED;

    return 1;
}

static void _tx_config(void)
{
    UCB0CTL1 |= UCTR + UCTXSTT;//transmit mode and send start condition
}

static void _rx_config(void)
{
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;//receiver mode
}

short i2c_reg_read(unsigned char reg, unsigned int size, i2c_data_read_callback func)
{
    if (!(state & BUS_INITIALIZED) || (state & BUS_BUSY))//bus off? or busy?
        return 0;

    data_read_func = func;
    data_size = size;
    ptr_data = buffer;
    send_register = reg;
    _tx_config();

    state  = BUS_BUSY + BUS_INITIALIZED;//bus on + busy + read mode

    return 1;
}

short i2c_reg_uchar_write(unsigned char reg, unsigned char value, i2c_data_write_callback func)
{
    if (!(state & BUS_INITIALIZED) || (state & BUS_BUSY))//bus off? or busy?
        return 0;

    data_write_func = func;
    data_size = 1;
    ptr_data = buffer;
    send_register = reg;
    buffer[0] = value;
    _tx_config();

    state = BUS_BUSY + WRITE_MODE + BUS_INITIALIZED;//bus on + busy + write mode

    return 1;
}

void i2c_rx_tx_int(void)
{
    if (!(state & REGISTER_SENT))
    {
        UCB0TXBUF = send_register;
        state |= REGISTER_SENT;
        return;
    }

    if (state & WRITE_MODE)
    {
        if (data_size)
        {
            UCB0TXBUF = *ptr_data;
            ptr_data++;
            data_size--;
        }
        else
        {
            state = 0;
            IFG2 &= ~UCB0TXIFG;
            UCB0CTL1 |= UCTXSTP;
            while (UCB0CTL1 & UCTXSTP);
            UCB0CTL1 |= UCSWRST;

            data_write_func(send_register);
        }
    }
    else//read mode
    {
        if (!(state & CONNECTION_RESTARDED))
        {
            IFG2 &= ~UCB0TXIFG;
            _rx_config();
            state |= CONNECTION_RESTARDED;
            return;
        }

        *ptr_data = UCB0RXBUF;
        data_size--;
        ptr_data++;
        if (!data_size)
        {
            state = 0;
            IFG2 &= ~UCB0TXIFG;
            UCB0CTL1 |= UCTXSTP;
            while (UCB0CTL1 & UCTXSTP);
            UCB0CTL1 |= UCSWRST;

            data_read_func(send_register, buffer);
        }
    }
}
