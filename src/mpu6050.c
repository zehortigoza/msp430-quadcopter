#include "main.h"

#define MPU6050_ADDR 0x0068//ADO = low

#define SAMPLES_TO_CALIBRATE 10

#define ACCEL_SCALE 8192.0
#define GYRO_SCALE 65.5

//registers
#define REG_WHO_AM_I 0x75
#define REG_PWR_MGMT_1 0x6B
#define REG_CONFIG 0x1A
#define REG_SMPRT_DIV 0x19
#define REG_GYRO_CONFIG 0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_INT_ENABLE 0x38
#define REG_INT_STATUS 0x3A
#define REG_ACCEL_XOUT_H 0x3B

typedef union _mpu6050_raw
{
    struct
    {
        unsigned char x_accel_h;
        unsigned char x_accel_l;
        unsigned char y_accel_h;
        unsigned char y_accel_l;
        unsigned char z_accel_h;
        unsigned char z_accel_l;
        unsigned char t_h;
        unsigned char t_l;
        unsigned char x_gyro_h;
        unsigned char x_gyro_l;
        unsigned char y_gyro_h;
        unsigned char y_gyro_l;
        unsigned char z_gyro_h;
        unsigned char z_gyro_l;
  } reg;
  struct
  {
      int x_accel;
      int y_accel;
      int z_accel;
      int temperature;//indicate that is first run of calibrate
      int x_gyro;
      int y_gyro;
      int z_gyro;
  } value;
} mpu6050_raw;

typedef struct _mpu6050_data
{
    float x_accel;
    float y_accel;
    float z_accel;
    float x_gyro;
    float y_gyro;
    float z_gyro;
} mpu6050_data;

static mpu6050_raw raw_offset;
static short calibrate;
static mpu6050_data sensor_data;

static void _int_enable_cb(unsigned char reg)
{
    //empty
}

static void _accel_config_cb(unsigned char reg)
{
    i2c_bus_init(MPU6050_ADDR);
    //enable data ready interruption
    i2c_reg_uchar_write(REG_INT_ENABLE, BIT0, _int_enable_cb);
}

static void _gyro_config_cb(unsigned char reg)
{
    i2c_bus_init(MPU6050_ADDR);
    //acell Full Scale Range = +-4g
    i2c_reg_uchar_write(REG_ACCEL_CONFIG, BIT3, _accel_config_cb);
}

static void _sampler_divider_cb(unsigned char reg)
{
    i2c_bus_init(MPU6050_ADDR);
    //gyro Full Scale Range = +-500 º/s
    i2c_reg_uchar_write(REG_GYRO_CONFIG, BIT3, _gyro_config_cb);
}

static void _config_cb(unsigned char reg)
{
    i2c_bus_init(MPU6050_ADDR);
    //divider = 4+1 = 5; 1k/5=200hz
    i2c_reg_uchar_write(REG_SMPRT_DIV, 4, _sampler_divider_cb);
}

static void _pw_mgmt_cb(unsigned char reg)
{
    i2c_bus_init(MPU6050_ADDR);
    //set Gyroscope Output Rate = 1k and config the low pass filter.
    i2c_reg_uchar_write(REG_CONFIG, BIT0, _config_cb);
}

static void _who_am_i_cb(unsigned char reg, unsigned char *data)
{
    if (data[0] != MPU6050_ADDR)
        return;
    i2c_bus_init(MPU6050_ADDR);
    //get out of sleep
    i2c_reg_uchar_write(REG_PWR_MGMT_1, 0, _pw_mgmt_cb);
}

int mpu6050_init(void)
{
    memset(&raw_offset, 0, sizeof(raw_offset));
    i2c_bus_init(MPU6050_ADDR);
    i2c_reg_read(REG_WHO_AM_I, 1, _who_am_i_cb);

    //config data ready interruption
    P1DIR |= BIT5;//input
    P2IE |= BIT5;//enable interruption
    P2IES &= ~BIT5;//0 = low to high | 1 = high to low
    P2IFG &= ~BIT5;//clean int

    return 0;
}

static inline void _swap(unsigned char *h, unsigned char *l)
{
    unsigned char temp;

    temp = *h;
    *h = *l;
    *l = temp;
}

static void _raw_swap(mpu6050_raw *raw)
{
    _swap(&raw->reg.t_h, &raw->reg.t_l);
    _swap(&raw->reg.x_accel_h, &raw->reg.x_accel_l);
    _swap(&raw->reg.y_accel_h, &raw->reg.y_accel_l);
    _swap(&raw->reg.z_accel_h, &raw->reg.z_accel_l);
    _swap(&raw->reg.x_gyro_h, &raw->reg.x_gyro_l);
    _swap(&raw->reg.y_gyro_h, &raw->reg.y_gyro_l);
    _swap(&raw->reg.z_gyro_h, &raw->reg.z_gyro_l);
}

static void _offset_remove(mpu6050_raw *raw)
{
    raw->value.x_accel -= raw_offset.value.x_accel;
    raw->value.y_accel -= raw_offset.value.y_accel;
    raw->value.z_accel -= raw_offset.value.z_accel;
    raw->value.x_gyro -= raw_offset.value.x_gyro;
    raw->value.y_gyro -= raw_offset.value.y_gyro;
    raw->value.z_gyro -= raw_offset.value.z_gyro;
}

static void _scale_calc(mpu6050_raw *raw)
{
    sensor_data.x_accel = raw->value.x_accel / ACCEL_SCALE;
    sensor_data.y_accel = raw->value.y_accel / ACCEL_SCALE;
    sensor_data.z_accel = raw->value.z_accel / ACCEL_SCALE;

    sensor_data.x_gyro = raw->value.x_gyro / GYRO_SCALE;
    sensor_data.y_gyro = raw->value.y_gyro / GYRO_SCALE;
    sensor_data.z_gyro = raw->value.z_gyro / GYRO_SCALE;
}

static void _raw_cb(unsigned char reg, unsigned char *data)
{
    static mpu6050_raw raw;

    memcpy(&raw, data, sizeof(raw));
    _raw_swap(&raw);

    if (calibrate > 0)
    {
        if (raw_offset.value.temperature == -1)
        {
            raw_offset.value.temperature = 0;
            raw_offset.value.x_accel = raw.value.x_accel;
            raw_offset.value.y_accel = raw.value.y_accel;
            raw_offset.value.z_accel = raw.value.z_accel;
            raw_offset.value.x_gyro = raw.value.x_gyro;
            raw_offset.value.y_gyro = raw.value.y_gyro;
            raw_offset.value.z_gyro = raw.value.z_gyro;
        }
        else
        {
            raw_offset.value.x_accel = (raw_offset.value.x_accel + raw.value.x_accel)/2;
            raw_offset.value.y_accel = (raw_offset.value.y_accel + raw.value.y_accel)/2;
            raw_offset.value.z_accel = (raw_offset.value.z_accel + raw.value.z_accel)/2;
            raw_offset.value.x_gyro = (raw_offset.value.x_gyro + raw.value.x_gyro)/2;
            raw_offset.value.y_gyro = (raw_offset.value.y_gyro + raw.value.y_gyro)/2;
            raw_offset.value.z_gyro = (raw_offset.value.z_gyro + raw.value.z_gyro)/2;
        }

        calibrate--;
        if (!calibrate)
            protocol_msg_send(CALIBRATE, 0);
    }

    _offset_remove(&raw);
    _scale_calc(&raw);

    //TODO transform to pitch, yaw and row
}

static void _status_cb(unsigned char reg, unsigned char *data)
{
   if (data[0] & BIT0)
   {
       i2c_bus_init(MPU6050_ADDR);
       i2c_reg_read(REG_ACCEL_XOUT_H, 14, _raw_cb);
   }
}

interrupt(PORT2_VECTOR) mpu_int(void)
{
    i2c_bus_init(MPU6050_ADDR);
    i2c_reg_read(REG_INT_STATUS, 1, _status_cb);
    P2IFG &= ~BIT5;//clean interruption flag
}

void mpu6050_calibrate(void)
{
    calibrate = SAMPLES_TO_CALIBRATE;
    raw_offset.value.temperature = -1;
}

void mpu6050_gyro_get(float *x, float *y, float *z)
{
    if (x)
        *x = sensor_data.x_gyro;
    if (y)
        *y = sensor_data.y_gyro;
    if (z)
        *z = sensor_data.z_gyro;
}

void mpu6050_accel_get(float *x, float *y, float *z)
{
    if (x)
        *x = sensor_data.x_accel;
    if (y)
        *y = sensor_data.y_accel;
    if (z)
        *z = sensor_data.z_accel;
}
