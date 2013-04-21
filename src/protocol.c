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
            int x, y, z;//only request

            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            x = atoi(pch);

            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            y = atoi(pch);

            pch = strtok(NULL, ";");
            if (!pch || pch[0] == '$')
                return;
            z = atoi(pch);

            protocol_msg_func(type, request, x, y, z);
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
    char *tx_buffer = radio_tx_buffer_get();
    va_list ap;

    va_start(ap, request);
    switch (type)
    {
        case PING:
        {
            //quad request to cel
            if (request)
                snprintf(tx_buffer, MAX_STRING, "^;%c;1;2;$", type);
            else
            {
                int num = va_arg(ap, int);
                num++;
                snprintf(tx_buffer, MAX_STRING, "^;%c;0;%d;$", type, num);
            }
            break;
        }
        case BATTERY:
        case RADIO_LEVEL:
        {
            int num = va_arg(ap, int);//only reply
            snprintf(tx_buffer, MAX_STRING, "^;%c;0;%d;$", type, num);
            break;
        }
        case GYRO:
        case ACCELEROMETER:
        {
            int x, y, z;//only reply
            x = va_arg(ap, int);
            y = va_arg(ap, int);
            z = va_arg(ap, int);
            snprintf(tx_buffer, MAX_STRING, "^;%c;0;%d;%d;%d;$", type, x, y, z);
            break;
        }
        case CALIBRATE:
        case MOVE:
        {
            //only reply
            snprintf(tx_buffer, MAX_STRING, "^;%c;0;$", type);
            break;
        }
        default:
        {
            va_end(ap);
            return 0;
        }
    }

    va_end(ap);
    return radio_send();
}
