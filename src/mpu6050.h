#ifndef MPU6050_H
#define MPU6050_H

int mpu6050_init(void);
void mpu6050_calibrate(void);
void mpu6050_gyro_get(float *x, float *y, float *z);
void mpu6050_accel_get(float *x, float *y, float *z);

#endif
