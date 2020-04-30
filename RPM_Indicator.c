/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC15W204S ------------------------------------*/
/* Tachometer Display 4 x 8 Segement display*/

//Keil Intel 8058
//6MHz
#define FOSC 18432000L
#define Cycle_Sec 1000;
#define Refresh (65536 - FOSC/12/1000)

#include "reg51.h"

sfr AUXR = 0x8e;
sfr INT_CLK0 = 0x8f;

sfr P4 = 0xc0;
sfr P5 = 0xc0;


unsigned char LED8[4];
unsigned char display_index;
unsigned int RPM;
unsigned int digit;
unsigned long RPM_timebase;
unsigned long RPM_timebase_capatured;


bit B_1ms;
unsigned char running;

sbit P32 =P3^2;
sbit P55 =P5^5;

// Segment_a P3.0
sbit Seg_a = P3^0;
// Segment_b  P5.4
sbit Seg_b = P5^4;
// Segment_c  P1.0
sbit Seg_c = P1^1;
// Segment_d P3.7
sbit Seg_d =P3^7;
// Segment_e P3.6
sbit Seg_e = P3^6;
// Segement_f P3.1
sbit Seg_f = P3^1;
// Segment_g P3.3
sbit Seg_g = P3^3;
// Segment_DP P1.1
sbit Seg_DP = P1^0;
// Digit1 P1.5
sbit Digit1 = P1^5;
// Digit2 P1.4
sbit Digit2= P1^4;
// Digit3 P1.3
sbit Digit3 = P1^3;
// Digit4 P1.2
sbit Digit4 = P1^2;


void delay()
{
    int i, j;

    for (i=0; i<10; i++)
    for (j=0; j<500; j++);
}


void Digit(unsigned char number)
{
    switch (number)
    {
        case 0:
        //Segment  Set a,b,c,d,e,f
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 1;
        Seg_f = 1;
        Seg_g = 0;
        break;

    
        case 1:
        //Segment  Set b,c
        Seg_a = 0;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 0;
        Seg_e = 0;
        Seg_f = 0;
        Seg_g = 0;
        break;

    
        case 2:
        //Segment  Set a,b,g,e,d
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 0;
        Seg_d = 1;
        Seg_e = 1;
        Seg_f = 0;
        Seg_g = 1;
        break;

    
        case 3:
        //Segment  Set a,b,c,d,g
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 0;
        Seg_f = 0;
        Seg_g = 1;
        break;

    
        case 4:
        //Segment  Set b,c,f,g
        Seg_a = 0;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 0;
        Seg_e = 0;
        Seg_f = 1;
        Seg_g = 1;
        break;

    
        case 5:
        //Segment  Set a,c,d,f,g
        Seg_a = 1;
        Seg_b = 0;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 0;
        Seg_f = 1;
        Seg_g = 1;
        break;

    
        case 6:
        //Segment  Set a,c,d,e,f,g
        Seg_a = 1;
        Seg_b = 0;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 1;
        Seg_f = 1;
        Seg_g = 1;
        break;

    
        case 7:
        //Segment  Set a,b,c
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 0;
        Seg_e = 0;
        Seg_f = 0;
        Seg_g = 0;
        break;

    
        case 8:
        //Segment  Set a,b,c,d,e,f,g
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 1;
        Seg_f = 1;
        Seg_g = 1;
        break;

    
        case 9:
        //Segment  Set a,b,c,d,f,g
        Seg_a = 1;
        Seg_b = 1;
        Seg_c = 1;
        Seg_d = 1;
        Seg_e = 0;
        Seg_f = 1;
        Seg_g = 1;
        break;
				
				default:
        //Segment  Set a,b,c,d,f,g
        Seg_a = 1;
        Seg_b = 0;
        Seg_c = 0;
        Seg_d = 1;
        Seg_e = 1;
        Seg_f = 1;
        Seg_g = 1;
        break;

    }

}
void Display(void)
{
unsigned int number;
Digit1 = 1;
Digit2 = 1;
Digit3 = 1;
Digit4 = 1;
if (RPM > 9999) digit = 5; 
if (RPM == 0) digit = 1;  
switch (digit)
    {
    case 1:
    number = RPM % 10;
    Digit(number);
		Digit4 = 0;
		break;
		case 2:
    number = (RPM / 10)	% 10;
    Digit(number);
		Digit3 = 0;
		break;
		case 3:
    number = (RPM / 100) % 10;
    Digit2 = 0;
    Digit(number);
		break;
		case 4:
    number = (RPM / 1000) % 10;
    Digit1 = 0;
    Digit(number);
		break;
		case 5: // Error
    
    Digit1 = 0;
		Digit2 = 0;
		Digit3 = 0;
		Digit4 = 0;
    Digit(0x11);
		break;
	}
    
}


void main()
{
unsigned int count; 
INT0 = 1;
IT0  = 0; // (1 falling only 0: Both)
EX0  = 1; //Ext Interrupt
EA   = 1;
	
TMOD = 0x01;
TH0 = Refresh>> 8;
TL0 = Refresh;
ET0 = 1;
TR0 = 1;
EA = 1;
RPM_timebase=0;
running = 0;
count=1;
digit=1;

    while (1)
    {
        
		Seg_DP =0;
    //RPM=count;
		
		count++;
		delay();
		
		//delay();
		//delay();
    if (count == 10000) count=0; 
		
    }
}

void timer0 (void) interrupt 1
{
    TH0 = Refresh>> 8;
		TL0 = Refresh;
    Display();
		digit++;
		if (digit == 5) digit =1;
    B_1ms =1;
		RPM_timebase++;
		if ( RPM_timebase > 1500) 
		{
		running = 0;
		RPM_timebase = 0;
		RPM=0;
		}
		
}

void ext_trig (void) interrupt 0
{
	// Pulse low once a rotation
	if (INT0 == 0) 
	{
		//RPM_timebase_capatured=RPM_timebase;
		if (running == 1) RPM=(6000/RPM_timebase)*10;
		running = 1;
		RPM_timebase=0;

	}
}

