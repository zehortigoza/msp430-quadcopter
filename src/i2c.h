#ifndef I2C_H
#define I2C_H

typedef void (*i2c_data_read_callback)(unsigned char reg, unsigned char *data);
typedef void (*i2c_data_write_callback)(unsigned char reg);

short i2c_bus_init(unsigned char address);
short i2c_reg_uchar_write(unsigned char reg, unsigned char value, i2c_data_write_callback func);
short i2c_reg_read(unsigned char reg, unsigned int size, i2c_data_read_callback func);

void i2c_rx_tx_int(void);

#endif
