#include "main.h"

static char _request_data = 0;//gyro = BIT0, ACCELEROMETER = BIT1

static void _msg_cb(Protocol_Msg_Type type, char request, ...)
{
    va_list ap;

    va_start(ap, request);
    switch (type)
    {
        case MOVE:
        {
            int x, y, z;
            x = va_arg(ap, int);
            y = va_arg(ap, int);
            z = va_arg(ap, int);

            //motors_velocity_get();
            //TODO calc
            //motors_velocity_set();

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
    motors_init();
    //TODO sensors_init(cb);
    procotol_init(_msg_cb);
}
