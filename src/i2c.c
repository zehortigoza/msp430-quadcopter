#include "main.h"

#define I2C_BUFFER_SIZE 32

/**
 * bit0 = busy
 * bit1 = reading(0) or writing(1) a register?
 * bit2 = register was transmitted?
 * bit3 = restart send *only used when reading a register
 * bit4 = bus in use
 */
static char state = 0;
static i2c_data_read_callback data_read_func;
static i2c_data_write_callback data_write_func;
static unsigned char send_register = 0;
static unsigned char buffer[I2C_BUFFER_SIZE];
static unsigned char *ptr_data;
static int data_size = 0;

int i2c_bus_init(unsigned char address)
{
    if (state & BIT4)
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
    IE2 |= UCB0TXIE;
    IE2 |= UCB0RXIE;
    state |= BIT4;

    return 1;
}

static void _tx_config(void)
{
    UCB0CTL1 |= UCTR + UCTXSTT;//transmit mode and send start condition
}

static void _rx_config(void)
{
    UCB0CTL1 &= ~UCTR;//receiver mode
    UCB0CTL1 |= UCTXSTT;//send start condition
}

static void _bus_shutdown(void)
{
    UCB0CTL1 |= UCSWRST;
    state = 0;
}

int i2c_reg_read(unsigned char reg, unsigned int size, i2c_data_read_callback func)
{
    if ((state & BIT4) != BIT4 && state & BIT0)
        return 0;
    state |= BIT0;
    _tx_config();
    data_read_func = func;
    data_size = size;
    ptr_data = buffer;
    send_register = reg;

    return 1;
}

int i2c_reg_uchar_write(unsigned char reg, unsigned char value, i2c_data_write_callback func)
{
    unsigned char bchar[1];
    bchar[0] = value;
    return i2c_reg_write(reg, bchar, 1, func);
}

int i2c_reg_write(unsigned char reg, unsigned char *value, unsigned int size, i2c_data_write_callback func)
{
    if ((state & BIT4) != BIT4)
        return 0;
    if (state & BIT0 || size > sizeof(buffer))
        return 0;
    state |= BIT0 + BIT1;
    _tx_config();
    data_write_func = func;
    data_size = size;
    memcpy(buffer, value, size);
    ptr_data = buffer;
    send_register = reg;
    
    return 1;
}

void i2c_rx_tx_int(void)
{
    if ((state & BIT2) != BIT2)//register not transmitted
    {
        UCB0TXBUF = send_register;
        state |= BIT2;
        return;
    }

    if (state & BIT1)//write mode
    {
        if (data_size)
        {
            UCB0TXBUF = *ptr_data;
            ptr_data++;
            data_size--;
        }
        else
        {
            char ret;
            UCB0CTL1 |= UCTXSTP;
            ret = data_write_func(send_register);
            while (UCB0CTL1 & UCTXSTP);
            IFG2 &= ~UCB0TXIFG;
            if (!ret)
                _bus_shutdown();
        }
    }
    else//read mode
    {
        if ((state & BIT3) != BIT3) //connection not restated
        {
            UCB0CTL1 |= UCTXSTP;
            while (UCB0CTL1 & UCTXSTP);
            IFG2 &= ~UCB0TXIFG;
            _rx_config();
            state |= BIT3;
            return;
        }

        *ptr_data = UCB0RXBUF;
        data_size--;
        ptr_data++;
        if (!data_size)
        {
            char ret;
            UCB0CTL1 |= UCTXSTP;
            ret = data_read_func(send_register, buffer);
            while (UCB0CTL1 & UCTXSTP);
            if (!ret)
                _bus_shutdown();
        }
    }
}
