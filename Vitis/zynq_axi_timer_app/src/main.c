#include "xil_printf.h"
#include "xil_io.h"

#define GPIO_BASE       0x41200000U
#define TIMER_BASE      0x42800000U

/* AXI GPIO registers */
#define GPIO_DATA       0x00U
#define GPIO_TRI        0x04U
#define GPIO2_DATA      0x08U
#define GPIO2_TRI       0x0CU

/* AXI Timer registers */
#define TIMER_TCSR0     0x00U
#define TIMER_TLR0      0x04U
#define TIMER_TCR0      0x08U

/* AXI Timer control/status bits */
#define TCSR_ENT        0x00000080U
#define TCSR_ENIT       0x00000040U
#define TCSR_LOAD       0x00000020U
#define TCSR_ARHT       0x00000010U
#define TCSR_TINT       0x00000100U

/* Timer clock = 50 MHz */
#define TIMER_CLOCK_HZ  50000000U

/* Initial period = 0.5 s */
#define PERIOD_FAST     10000000U
#define PERIOD_NORMAL   25000000U
#define PERIOD_SLOW     50000000U

/* BTN_SW mapping:
 * bit 0 = BTN0 - faster
 * bit 1 = BTN1 - slower
 * bit 2 = BTN2 - STOP
 * bit 3 = BTN3 - RESUME
 * bit 4 = SW0  - direction
 */

volatile unsigned int led_position = 0;
volatile unsigned int running = 1;
volatile unsigned int period = PERIOD_NORMAL;


/* Set AXI Timer period */
void timer_set_period(unsigned int clocks)
{
    unsigned int control;

    /* Stop timer */
    control = Xil_In32(TIMER_BASE + TIMER_TCSR0);
    control &= ~TCSR_ENT;
    Xil_Out32(TIMER_BASE + TIMER_TCSR0, control);

    /* Load new period */
    Xil_Out32(TIMER_BASE + TIMER_TLR0,
              0xFFFFFFFFU - clocks + 1U);

    /* Load and start timer */
    Xil_Out32(TIMER_BASE + TIMER_TCSR0,
              TCSR_LOAD);

    Xil_Out32(TIMER_BASE + TIMER_TCSR0,
              TCSR_ENT | TCSR_ARHT | TCSR_ENIT);
}


/* Move LED */
void update_led(unsigned int direction)
{
    if (direction == 0)
    {
        /* Forward: LED0 -> LED1 -> LED2 -> LED3 */
        led_position++;

        if (led_position >= 4)
            led_position = 0;
    }
    else
    {
        /* Backward: LED3 -> LED2 -> LED1 -> LED0 */
        if (led_position == 0)
            led_position = 3;
        else
            led_position--;
    }

    Xil_Out32(GPIO_BASE + GPIO_DATA,
              (1U << led_position));
}


int main()
{
    unsigned int buttons;
    unsigned int previous_buttons = 0;
    unsigned int direction;

    xil_printf("\r\n");
    xil_printf("=================================\r\n");
    xil_printf(" Zynq AXI Timer Running LED\r\n");
    xil_printf("=================================\r\n");

    /* GPIO Channel 1: LED0..LED3 = outputs */
    Xil_Out32(GPIO_BASE + GPIO_TRI, 0x00000000U);

    /* GPIO Channel 2: BTN0..BTN3 + SW0 = inputs */
    Xil_Out32(GPIO_BASE + GPIO2_TRI, 0x0000001FU);

    /* Start with LED0 */
    led_position = 0;
    Xil_Out32(GPIO_BASE + GPIO_DATA, 0x00000001U);

    /* Start AXI Timer */
    timer_set_period(period);

    xil_printf("Timer clock: 50 MHz\r\n");
    xil_printf("Initial period: 0.5 s\r\n");
    xil_printf("BTN0 = faster\r\n");
    xil_printf("BTN1 = slower\r\n");
    xil_printf("BTN2 = stop\r\n");
    xil_printf("BTN3 = resume\r\n");
    xil_printf("SW0  = direction\r\n");

    while (1)
    {
        /*
         * Read buttons and switch.
         */
        buttons = Xil_In32(GPIO_BASE + GPIO2_DATA) & 0x1FU;

        /*
         * SW0 controls direction.
         */
        direction = (buttons >> 4) & 1U;

        /*
         * Process button press only once.
         */
        if (buttons != previous_buttons)
        {
            /* BTN0 - faster */
            if (buttons & 0x01U)
            {
                if (period > PERIOD_FAST)
                {
                    period /= 2;

                    if (period < PERIOD_FAST)
                        period = PERIOD_FAST;

                    timer_set_period(period);

                    xil_printf("Speed UP, period = %u\r\n",
                               period);
                }
            }

            /* BTN1 - slower */
            if (buttons & 0x02U)
            {
                if (period < PERIOD_SLOW)
                {
                    period *= 2;

                    if (period > PERIOD_SLOW)
                        period = PERIOD_SLOW;

                    timer_set_period(period);

                    xil_printf("Speed DOWN, period = %u\r\n",
                               period);
                }
            }

            /* BTN2 - STOP */
            if (buttons & 0x04U)
            {
                running = 0;

                xil_printf("STOP\r\n");
            }

            /* BTN3 - RESUME */
            if (buttons & 0x08U)
            {
                running = 1;

                xil_printf("RESUME\r\n");
            }

            previous_buttons = buttons;
        }

        /*
         * Check AXI Timer interrupt/status flag.
         * No software delay is used.
         */
        if (Xil_In32(TIMER_BASE + TIMER_TCSR0) & TCSR_TINT)
        {
            /* Clear timer interrupt/status */
            Xil_Out32(TIMER_BASE + TIMER_TCSR0,
                      TCSR_TINT);

            /*
             * Move LED only when not stopped.
             */
            if (running)
            {
                update_led(direction);

                xil_printf("LED%u\r\n",
                           led_position);
            }
        }
    }

    return 0;
}