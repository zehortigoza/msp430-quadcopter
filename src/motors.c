#include "main.h"

//page 366 *The number of timer counts in the period is TACCR0+1
#define COUNTER_TO_5MS 625-1//625hz = 5ms
#define COUNTER_TO_1MS 125-1//125hz = 1ms

static unsigned char motor_index = 0;

//index 0 = front right, 1 = front left, 2 = back right, 3 = back left
static int motors_tricks[4];

int motors_init(void)
{
    TACTL = TASSEL_2;//source of timerA = smlck
    TACTL |= ID_3;//divide clock per 8, 1mhz/8 = 125000hz
    TACTL |= MC_1;//up mode

    TACCTL0 = CCIE;//enable interruption compare 1
    TACCTL1 = CCIE;//enable interruption compare 2

    TACCR0 = COUNTER_TO_5MS;
    TACCR1 = 0;

    return 0;
}

void motors_velocity_set(unsigned char fr, unsigned char fl, unsigned char br, unsigned char bl)
{
    motors_tricks[0] = (COUNTER_TO_1MS * fr / 255) + COUNTER_TO_1MS - 1;
    motors_tricks[1] = (COUNTER_TO_1MS * fl / 255) + COUNTER_TO_1MS - 1;
    motors_tricks[2] = (COUNTER_TO_1MS * br / 255) + COUNTER_TO_1MS - 1;
    motors_tricks[3] = (COUNTER_TO_1MS * bl / 255) + COUNTER_TO_1MS - 1;
}

static int _power_calc(int count)
{
    count = count - COUNTER_TO_1MS + 1;
    count = 255 * count / 125;
    return count;
}

void motors_velocity_get(unsigned char *fr, unsigned char *fl, unsigned char *br, unsigned char *bl)
{
    if (fr)
        *fr = _power_calc(motors_tricks[0]);
    if (fl)
        *fl = _power_calc(motors_tricks[1]);
    if (br)
        *br = _power_calc(motors_tricks[2]);
    if (bl)
        *bl = _power_calc(motors_tricks[3]);
}

static void _motor_pwm_up(void)
{
    //TODO SET PIN UP
    TACCR1 = motors_tricks[motor_index];
}

interrupt(TIMER0_A1_VECTOR) timer_a1_int(void)
{
    //TODO SET PIN DOWN
}

interrupt(TIMER0_A0_VECTOR) timer_a0_int(void)
{
    _motor_pwm_up();
    motor_index++;
    if(motor_index == 4)
        motor_index = 0;
    TACCR0 = COUNTER_TO_5MS;
}
