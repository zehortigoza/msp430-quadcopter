#ifndef I2C_H
#define I2C_H

/*
 * If callback functions return 0, i2c bus is release
 * */
typedef char (*i2c_data_read_callback)(unsigned char reg, unsigned char *data);
typedef char (*i2c_data_write_callback)(unsigned char reg);

int i2c_bus_init(unsigned char address);
int i2c_reg_write(unsigned char reg, unsigned char *value, unsigned int size, i2c_data_write_callback func);
int i2c_reg_read(unsigned char reg, unsigned int size, i2c_data_read_callback func);

void i2c_rx_tx_int(void);

#endif
