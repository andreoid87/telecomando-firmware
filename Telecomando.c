#define IR_CODE_L1      0b010010010000
#define IR_CODE_L2      0b010100001000
#define IR_CODE_L3      0b010010100000
#define IR_CODE_L4      0b010010001000
#define IR_CODE_L5      0b010100000010
#define IR_CODE_MUTE    0b010010000100
#define IR_CODE_VOL_UP  0b010100010000
#define IR_CODE_VOL_DN  0b010100100000
#define IR_CODE_STBY    0b010100110000

#define IR_ON           0   // IR anode active low on the base
#define IR_OFF          1
#define PIN_ACTIVE      0   // inputs are active low
#define PIN_INACTIVE    1

#define EDGE_RISING     1
#define EDGE_FALLING    0

#define IR_BITS         12
#define IR_MSB_MASK     0b100000000000
#define DEBOUNCE_MS     5

sbit anodo  at RA2_bit; // to be reassigned if needed
sbit l1l2   at RB4_bit;
sbit l3l4   at RB5_bit;
sbit l5mute at RB6_bit;
sbit vol    at RB7_bit;

sbit TRISanodo at TRISA2_bit;

static void trasmetti(unsigned int ir_code);
static void uno(void);
static void zero(void);
static bit read_ra1_stable(void);

void main()
{
    TRISanodo = 0;
    TRISA0_bit = 0;
    TRISA1_bit = 0;
    TRISB = 0xFF;

    RBIE_bit = 1;                 // enable PORTB change interrupt
    OPTION_REG.INTEDG = EDGE_FALLING;
    OPTION_REG &= 0b01111111;     // enable internal pull-ups
    GIE_bit = 1;                  // enable global interrupts

    RA0_bit = 0;
    RA1_bit = 0;
    anodo = IR_OFF;

    while (1)
    {
        Delay_ms(10); // avoid fast transitions sleep->on
        asm { SLEEP };
        asm { NOP };
    }
}

void interrupt() iv 0x0004 ics ICS_OFF
{
    unsigned char portb_snapshot;

    portb_snapshot = PORTB; // clear PORTB mismatch condition

    while (l1l2 == PIN_ACTIVE)
    {
        delay_ms(DEBOUNCE_MS);
        if (l1l2 != PIN_ACTIVE)
        {
            break;
        }
        RA0_bit = 1;
        delay_us(5);
        if (l1l2 == PIN_INACTIVE)
        {
            trasmetti(IR_CODE_L1);
        }
        if (l1l2 == PIN_ACTIVE)
        {
            trasmetti(IR_CODE_L2);
        }
        RA0_bit = 0;
        delay_ms(300);
    }

    while (l3l4 == PIN_ACTIVE)
    {
        delay_ms(DEBOUNCE_MS);
        if (l3l4 != PIN_ACTIVE)
        {
            break;
        }
        RA0_bit = 1;
        delay_us(5);
        if (l3l4 == PIN_INACTIVE)
        {
            trasmetti(IR_CODE_L3);
        }
        if (l3l4 == PIN_ACTIVE)
        {
            trasmetti(IR_CODE_L4);
        }
        RA0_bit = 0;
        delay_ms(300);
    }

    while (l5mute == PIN_ACTIVE)
    {
        delay_ms(DEBOUNCE_MS);
        if (l5mute != PIN_ACTIVE)
        {
            break;
        }
        RA0_bit = 1;
        delay_us(5);
        if (l5mute == PIN_INACTIVE)
        {
            trasmetti(IR_CODE_L5);
        }
        if (l5mute == PIN_ACTIVE)
        {
            trasmetti(IR_CODE_MUTE);
            delay_ms(1000);
        }
        RA0_bit = 0;
        delay_ms(300);
    }

    while (vol == PIN_ACTIVE)
    {
        delay_ms(DEBOUNCE_MS);
        if (vol != PIN_ACTIVE)
        {
            break;
        }
        RA0_bit = 1;
        delay_us(5);
        if (vol == PIN_INACTIVE)
        {
            trasmetti(IR_CODE_VOL_UP);
        }
        if (vol == PIN_ACTIVE)
        {
            RA0_bit = 0;
            if (read_ra1_stable())
            {
                trasmetti(IR_CODE_STBY);
                delay_ms(1000);
            }
            else
            {
                trasmetti(IR_CODE_VOL_DN);
            }
            TRISA1_bit = 0;
        }
        RA0_bit = 0;
        RA1_bit = 0;
        delay_us(5);
    }

    anodo = IR_OFF;
    RBIF_bit = 0;
}

static bit read_ra1_stable(void)
{
    bit sample1;
    bit sample2;

    TRISA1_bit = 1;
    delay_us(10);
    sample1 = RA1_bit;
    delay_us(10);
    sample2 = RA1_bit;
    TRISA1_bit = 0;

    if ((sample1 == 0) && (sample2 == 0))
    {
        return 1;
    }
    return 0;
}

static void trasmetti(unsigned int ir_code)
{
    unsigned char i;

    for (i = 0; i < IR_BITS; i++)
    {
        if (ir_code & IR_MSB_MASK)
        {
            uno();
        }
        else
        {
            zero();
        }
        ir_code = ir_code << 1;
    }

    delay_ms(64);
}

static void zero(void)
{
    unsigned char j;

    for (j = 0; j < 16; j++)
    {
        anodo = IR_ON;
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        anodo = IR_OFF;
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
    }

    for (j = 0; j < 173; j++)
    {
        ;
    }
    asm { NOP };
    asm { NOP };
}

static void uno(void)
{
    unsigned char k;

    for (k = 0; k < 48; k++)
    {
        anodo = IR_ON;
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        anodo = IR_OFF;
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
        asm { NOP };
    }

    for (k = 0; k < 52; k++)
    {
        ;
    }
    asm { NOP };
    asm { NOP };
    asm { NOP };
    asm { NOP };
}
