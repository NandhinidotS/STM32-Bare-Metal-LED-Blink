#include <stdint.h>

/* GPIO and clock registers */
#define RCC_AHB1ENR   (*(volatile uint32_t *)0x40023830UL)
#define GPIOA_MODER   (*(volatile uint32_t *)0x40020000UL)
#define GPIOA_ODR     (*(volatile uint32_t *)0x40020014UL)

/* SysTick registers (used for an exact millisecond delay) */
#define SYST_CSR      (*(volatile uint32_t *)0xE000E010UL)
#define SYST_RVR      (*(volatile uint32_t *)0xE000E014UL)
#define SYST_CVR      (*(volatile uint32_t *)0xE000E018UL)

#define LED_MASK      (1UL << 5U)   /* LD2 is on PA5 */

void SystemInit(void)
{
}

/* Exact delay in ms, assuming the default 16 MHz clock */
static void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        SYST_RVR = 16000U - 1U;   /* 16000 clocks = 1 ms */
        SYST_CVR = 0U;            /* clear the counter   */
        SYST_CSR = 5U;            /* enable, use CPU clock, no interrupt */
        while (!(SYST_CSR & (1UL << 16)))
        {
            /* wait until the 1 ms count is finished */
        }
    }
    SYST_CSR = 0U;                /* stop the timer */
}

int main(void)
{
    uint32_t i;

    /* Clock ON for GPIOA */
    RCC_AHB1ENR |= (1UL << 0);

    /* PA5 as output: clear both mode bits, then set bit 10 (mode 01) */
    GPIOA_MODER &= ~(3UL << (5U * 2U));
    GPIOA_MODER |=  (1UL << (5U * 2U));

    while (1)
    {
        /* 1. SET: LED ON for 4 sec */
        GPIOA_ODR |= LED_MASK;
        delay_ms(4000);

        /* 2. CLEAR: LED OFF for 4 sec */
        GPIOA_ODR &= ~LED_MASK;
        delay_ms(4000);

        /* 3. TOGGLE: fast blink for about 4 sec (41 flips x 100 ms) */
        for (i = 0; i < 41; i++)
        {
            GPIOA_ODR ^= LED_MASK;
            delay_ms(100);
        }

        /* 4. CHECK: if the LED bit is still 1, CLEAR it */
        if (GPIOA_ODR & LED_MASK)
        {
            GPIOA_ODR &= ~LED_MASK;
        }

        /* the while(1) loop now starts again from step 1 */
    }
}
