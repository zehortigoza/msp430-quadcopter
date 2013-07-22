#include "main.h"

static protocol_msg_callback protocol_msg_func = NULL;

//^;<type>;<1=request | 0=reply>;param1;...;paramn;$
//cel -> quad
static void _radio_cb(char *text)
{
    Protocol_Msg_Type type;
    char request;
    char *pch = strtok(text, ";");

    if (!pch || pch[0] != '^')
        return;

    //type
    pch = strtok(NULL, ";");
    if (!pch || pch[0] == '$')
        return;
    type = pch[0];

    //request/reply
    pch = strtok(NULL, ";");
    if (!pch || pch[0] == '$')
        return;
    request = pch[0];

    switch (type)
    {
        case MOVE:
        {
            int num;//only request
            Protocol_Axis axis;

            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            axis = pch[0];

            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            num = atoi(pch);

            protocol_msg_func(type, request, axis, num);
            break;
        }
        case PING:
        {
            int num;
            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            num = atoi(pch);
            protocol_msg_func(type, request, num);
            break;
        }
        case BATTERY:
        case RADIO_LEVEL:
        case CALIBRATE:
        case GYRO:
        case ACCELEROMETER:
        {
            protocol_msg_func(type, request);
            break;
        }
    }
}

void procotol_init(protocol_msg_callback cb)
{
    radio_init(_radio_cb);
    protocol_msg_func = cb;
}

int protocol_msg_send(Protocol_Msg_Type type, char request, ...)
{
    char tx_buffer[MAX_STRING];
    va_list ap;

    va_start(ap, request);
    switch (type)
    {
        case PING:
        {
            //quad request to cel
            if (request)
                sprintf(tx_buffer, "^;%c;1;2;$\n", type);
            else
            {
                int num = va_arg(ap, int);
                sprintf(tx_buffer, "^;%c;0;%d;$\n", type, num);
            }
            break;
        }
        case BATTERY:
        case RADIO_LEVEL:
        {
            int num = va_arg(ap, int);//only reply
            sprintf(tx_buffer, "^;%c;0;%d;$\n", type, num);
            break;
        }
        case GYRO:
        case ACCELEROMETER:
        {
            double x, y, z;//only reply
            x = va_arg(ap, double);
            y = va_arg(ap, double);
            z = va_arg(ap, double);
            sprintf(tx_buffer, "^;%c;0;%.3f;%.3f;%.3f;$\n", type, x, y, z);
            break;
        }
        case CALIBRATE:
        case MOVE:
        {
            //only reply
            sprintf(tx_buffer, "^;%c;0;$\n", type);
            break;
        }
        default:
        {
            va_end(ap);
            return 0;
        }
    }

    va_end(ap);
    return radio_send(tx_buffer);
}
