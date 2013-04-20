#include "main.h"

#define COUNTER_TO_5MS 625-1 //625hz = 5ms

static char motor = 0;

static char front_right = 0;//1
static char front_left = 0;//2
static char back_right = 0;//3
static char back_left = 0;//4

int motors_init(void)
{
    //config timerA
    TACTL = TASSEL_2;//source of timerA = smlck
    TACTL |= ID_3;//divide clock per 8, 1mhz/8 = 125000hz
    TACTL |= MC_1;//up mode

    TACCTL0 = CCIE;//enable interruption compare 1
    TACCTL1 = CCIE;//enable interruption compare 2

    TACCR0 = COUNTER_TO_5MS;
    TACCR1 = 0;

    return 0;
}

interrupt(TIMER0_A1_VECTOR) timer_a1_int(void)
{
    P1OUT = 0;
    //TODO SET PIN DOWN
}

static void _motor_pwm_up(void)
{
    unsigned int count = 125;//1ms

    //1ms = min velocity
    //2ms = max velocity
    //TODO SET PIN UP
    //TODO realize calc to count

    TACCR1 = count;
}

interrupt(TIMER0_A0_VECTOR) timer_a0_int(void)
{
    motor++;
    _motor_pwm_up();
    if(motor == 4)
        motor = 0;
    TACCR0 = COUNTER_TO_5MS;
}
