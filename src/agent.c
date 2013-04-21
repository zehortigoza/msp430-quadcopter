#include "main.h"

static char _request_data = 0;//gyro = BIT0, ACCELEROMETER = BIT1
static unsigned char z_value;

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
            _request_data ^= BIT0;
            break;
        }
        case ACCELEROMETER:
        {
            _request_data ^= BIT1;
            break;
        }
        case CALIBRATE:
        {
            //TODO calibrate
            protocol_msg_send(type, 0);
            break;
        }
        case RADIO_LEVEL:
        {
            //TODO send at comand to xbee to get radio level
            break;
        }
    }
    va_end(ap);
}

void agent_init(void)
{
    z_value = 0;
    motors_init();
    //TODO sensors_init(cb);
    procotol_init(_msg_cb);
}
