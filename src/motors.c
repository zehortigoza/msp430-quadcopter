#include "main.h"

//page 366 *The number of timer counts in the period is TACCR0+1
#define COUNTER_TO_2MS 250-1
#define COUNTER_TO_4MS 500-1

#define COUNTER_TO_1MS_ 125

static unsigned char motor_index = 0;

//index 0 = front right, 1 = front left, 2 = back right, 3 = back left
static unsigned short motors_tricks[4];

static unsigned char motor_state_get(unsigned char index)
{
    if (index == 0)
        return P2OUT & BIT3;
    else
        return 0;
}

static void motor_state_set(unsigned char index, unsigned char up)
{
    if (up)
    {
        if (index == 0)
            P2OUT |= BIT3;
    }
    else
    {
        if (index == 0)
            P2OUT &= ~BIT3;
    }
}

interrupt(TIMER0_A0_VECTOR) timer_a0_int(void)
{
    if (motor_state_get(motor_index))
    {
        motor_state_set(motor_index, 0);
        TACCR0 = COUNTER_TO_2MS;
    }
    else
    {
        if (motors_tricks[motor_index] && motor_index == 0)//TODO remove "motor_index == 0" when add the other motors
        {
            motor_state_set(motor_index, 1);
            TACCR0 = motors_tricks[motor_index];
            return;
        }
        else
            TACCR0 = COUNTER_TO_4MS;
    }

    motor_index++;
    if (motor_index == 4)
        motor_index = 0;
}

int motors_init(void)
{
    //motors pins setup
    P2DIR |= BIT3;//pin of motor fr in OUTUPT mode
    P2OUT &= ~BIT3;//set pin to 0

    motors_tricks[0] = 0;
    motors_tricks[1] = 0;
    motors_tricks[2] = 0;
    motors_tricks[3] = 0;

    TACTL = TASSEL_2 + MC_1 + ID_3;//smlck + up mode + /8
    TACCTL0 = CCIE;//enable interruption compare 1
    TACCR0 = COUNTER_TO_2MS;

    return 0;
}

static unsigned short _tricks_calc(unsigned char value)
{
    if (!value)
        return 0;
    return ((COUNTER_TO_1MS_ * value) / 255) + COUNTER_TO_1MS_;
}

/*
 * 255 = 2ms up
 * 1 = 1ms up
 * 0 = motor turn off
 */
void motors_velocity_set(unsigned char fr, unsigned char fl, unsigned char br, unsigned char bl)
{
    _tricks_calc(fr);
    _tricks_calc(fl);
    _tricks_calc(br);
    _tricks_calc(bl);
}

static unsigned char _power_calc(unsigned short count)
{
    if (!count)
        return 0;
    return ((count - COUNTER_TO_1MS_) * 255) / COUNTER_TO_1MS_;
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
