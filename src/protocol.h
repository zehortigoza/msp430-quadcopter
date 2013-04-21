#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef enum
{
  PING = 'p',
  BATTERY = 'b',
  RADIO_LEVEL = 'r',
  MOVE = 'm',
  GYRO = 'g',
  ACCELEROMETER = 'a',
  CALIBRATE = 'c'
} Protocol_Msg_Type;

typedef void (*protocol_msg_callback)(Protocol_Msg_Type type, char request, ...);

void procotol_init(protocol_msg_callback cb);
int protocol_msg_send(Protocol_Msg_Type type, char request, ...);

#endif
