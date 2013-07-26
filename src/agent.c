#include "main.h"

#define COUNTER_500MS 62500

static unsigned char z_value;
static int timer = 0;

static void _timer_b_config(void)
{
    //config timer B
    TA1CTL = TASSEL_2;//source of timerB = smlck
    TA1CTL |= ID_3;//divide clock per 8, 1mhz/8 = 125000hz = 125k
    TA1CTL |= MC_1;//Up mode: the timer counts up to TBCL0

    TA1CCTL0 = CCIE;//enable interruption compare 1
    TA1CCR0 = COUNTER_500MS;
}

static void _timer_b_reset(void)
{
    TA1CTL = TACLR;
    _timer_b_config();
}

/*
 BL\        /FL
    \      /
     \    /         x
      \  /          ^
       \/           |
       /\           |
      /  \    y<-----
     /    \
    /      \
 BR/        \FR
*/
static void _msg_cb(Protocol_Msg_Type type, char request, ...)
{
    va_list ap;

    va_start(ap, request);
    switch (type)
    {
        case MOVE:
        {
            Protocol_Axis axis;
            int num;

            _timer_b_reset();

            axis = va_arg(ap, Protocol_Axis);
            num = va_arg(ap, int);

            switch (axis)
            {
                case AXIS_Z:
                {
                    //num [0-255]
                    z_value = num;
                    motors_velocity_set(z_value, z_value, z_value, z_value);
                    break;
                }
                case AXIS_X:
                {
                    //num [-3-3]
                    if (!num)
                        motors_velocity_set(z_value, z_value, z_value, z_value);
                    else if (num < 0)//left
                    {
                        //TODO calc
                        //slow rotations on both left motors
                        //fast rotations on both right motors
                    } else//right
                    {
                        //TODO calc
                        //fast rotations on both left motors
                        //slow rotations on both right motors
                    }
                    //calc all for values basead variables but dont set
                    //only send to motors
                    break;
                }
                case AXIS_Y:
                {
                    //num [-3-3]
                    if (!num)
                        motors_velocity_set(z_value, z_value, z_value, z_value);
                    else if (num > 0)//front
                    {
                        //slow rotations on both front motors
                        //fast rotations on both back motors
                    }
                    else
                    {
                        //fast rotations on both front motors
                        //slow rotations on both back motors
                    }
                    break;
                }
                case AXIS_ROTATE:
                {
                    if (!num)
                        motors_velocity_set(z_value, z_value, z_value, z_value);
                    else if (num > 0)
                    {
                        //fast rotations on fl and br
                        //slow rotations on fr and bl
                    }
                    else
                    {
                        //slow rotations on fl and br
                        //fast rotations on fr and bl
                    }
                    break;
                }
            }

            protocol_msg_send(type, 0);
            break;
        }
        case PING:
        {
            int num = va_arg(ap, int);
            protocol_msg_send(type, 0, num+1);
            break;
        }
        case BATTERY:
        {
            //TODO setup AD
            break;
        }
        case GYRO:
        {
            float x, y, z;
            mpu6050_gyro_get(&x, &y, &z);
            protocol_msg_send(type, 0, x, y, z);
            break;
        }
        case ACCELEROMETER:
        {
            float x, y, z;
            mpu6050_accel_get(&x, &y, &z);
            protocol_msg_send(type, 0, x, y, z);
            break;
        }
        case CALIBRATE:
        {
            mpu6050_calibrate();
            break;
        }
        case RADIO_LEVEL:
        {
            //TODO send at comand to xbee to get radio level
            break;
        }
        default:
        {
            break;
        }
    }
    va_end(ap);
}

void agent_init(void)
{
    z_value = 0;
    motors_init();
    procotol_init(_msg_cb);
    mpu6050_init();
    _timer_b_config();
}

interrupt(TIMER1_A0_VECTOR) timer_b0_int(void)
{
    if (timer == 10)
        mpu6050_calibrate();
    timer++;

    //remove moviments
    motors_velocity_set(z_value, z_value, z_value, z_value);
    TA1CCR0 = COUNTER_500MS;
}
