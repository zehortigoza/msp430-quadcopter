#ifndef MPU6050_H
#define MPU6050_H

int mpu6050_init(void);
void mpu6050_calibrate(void);
void mpu6050_gyro_get(double *x, double *y, double *z);
void mpu6050_accel_get(double *x, double *y, double *z);

#endif
