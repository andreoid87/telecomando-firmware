#define L1           0b010010010000
#define L2           0b010100001000
#define L3           0b010010100000
#define L4           0b010010001000
#define L5           0b010100000010
#define MUTE         0b010010000100
#define VOLPIU       0b010100010000
#define VOLMENO      0b010100100000
#define STANDBY      0b010100110000
#define ON           0         // nella basetta ON in infrosso è a massa
#define OFF          1        //  nel telecomando si definirà in seguito
#define PON          0         // nella basetta ON in infresso è a massa
#define POFF         1        //  nel telecomando si definirà in seguito
#define SALITA       1
#define DISCESA      0

sbit anodo        at RA2_bit; //bit da ridefinire
sbit l1l2         at RB4_bit;
sbit l3l4         at RB5_bit;
sbit l5mute       at RB6_bit;
sbit vol          at RB7_bit;

sbit TRISanodo      at TRISA2_bit; //bit da ridefinire

void trasmetti(int n);
void uno();
void zero();

void main() 
{
TRISanodo          = 0; // anodo è impostato come output
TRISA0_bit = 0;
TRISA1_bit = 0;
TRISB              = 0xFF;
RBIE_bit           = 1;       // attivo l'interrupt per il risveglio dal modo SLEEP
OPTION_REG.INTEDG  = DISCESA;   // imposto l'interrupt d'accensione sul fronte di salita
OPTION_REG        &= 0b01111111;   // attivo i resistori interni di pull-up
GIE_bit              = 1;       // attivo la funzione di interrupt generale
RA0_bit = 0;
RA1_bit = 0;
anodo = OFF;      // da invertire sul telecomando
while(1)            // Endless loop
                    {
                    Delay_ms(10);     //evito transizioni troppo veloci da sleep a on
                    asm{SLEEP};       // modalità basso consumo
                    asm{NOP};         // consigliato dal datasheet
                    }
}

// da sistemare


void interrupt() iv 0x0004 ics ICS_OFF {
//supponendo l'incremento a 1MHZ, un incremento ogni 1us, per far scattare l'overflow
//dopo 13us bisogna settare il timer a 8 bit a 256 -13 +2(cicli inibiti postscrittura)
 while(l1l2 == PON)
          {
          RA0_bit = 1;
          delay_us(5);
          if (l1l2 == POFF) trasmetti(L1);
          if (l1l2 == PON ) trasmetti(L2);
          RA0_bit = 0;
          delay_ms(300);
          }
 while (l3l4 == PON)
          {
          RA0_bit = 1;
          delay_us(5);
          if (l3l4 == POFF) trasmetti(L3);
          if (l3l4 == PON ) trasmetti(L4);
          RA0_bit = 0;
          delay_ms(300);
          }
 while (l5mute == PON)
          {
          RA0_bit = 1;
          delay_us(5);
          if (l5mute == POFF) trasmetti(L5);
          if (l5mute == PON)
             {
             trasmetti(MUTE);
             delay_ms(1000);
             }
          RA0_bit = 0;
          delay_ms(300);
          }
 while(vol == PON)
          {
          RA0_bit = 1;

          delay_us(5);
          if (vol == POFF)   trasmetti(VOLPIU);
          if (vol == PON)
             {
             RA0_bit = 0;
             TRISA1_bit = 1;
             if (RA1_bit == 0)
                {
                trasmetti(STANDBY);
                delay_ms(1000);
                }
             else trasmetti(VOLMENO);
             TRISA1_bit = 0;
             }
          RA0_bit = 0;
          RA1_bit = 0;
          delay_us(5);
          }
 anodo = OFF;
 RBIF_bit = 0;
}


void trasmetti(int n)
{
int i;
for(i=0; i<12; i++)
         {
         if(n & 0b100000000000) uno();
         else zero();
         n=n<<1;
         }
delay_ms(64); //208a
}

void zero()
{
char j;
for(j=0;j<16;j++)
                 {
                 anodo=ON;
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 anodo=OFF;
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 }
                 for(j=0;j<173;j++);
                 asm{NOP};
                 asm{NOP};
}


void uno()
{
char k;
for(k=0;k<48;k++)
                 {
                 anodo=ON;
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 anodo=OFF;
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 asm{NOP};
                 }
for(k=0;k<52;k++);
asm{NOP};
asm{NOP};
asm{NOP};
asm{NOP};
}