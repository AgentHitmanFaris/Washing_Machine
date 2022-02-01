/*
 * Author: Agent_Hitman_Faris
 *
 * Created with love
 */
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#define _XTAL_FREQ 8000000
#define RS PORTCbits.RC0
#define EN PORTCbits.RC1
#define D4 PORTDbits.RD0
#define D5 PORTDbits.RD1
#define D6 PORTDbits.RD2
#define D7 PORTDbits.RD3

#include <stdlib.h>
#include <stdio.h> //header file for sprintf
#include <xc.h>
#include "lcd.h"
void main()
{
PORTDbits.RD4 = 0;
PORTDbits.RD5 = 0;

ANSEL = 0x00; 
TRISC = 0x00;
TRISD = 0x00;
ANSELH = 0x00;
TRISB = 0xFF;
OSCCON = 0x71;
PORTA = 0x00;
PORTB = 0xFF;
PORTC = 0x00;
PORTD = 0x00;

//Interupt
INTCONbits.INTE =1;
INTCONbits.GIE =1;
OPTION_REGbits.INTEDG = 0 ;
void inter();

int up=1,enter=0,stop=0,menu=2,a=0,b=0,c=0,d=0;
//motor
PORTDbits.RD4 = 0;
PORTDbits.RD5 = 0;
Lcd_Init();
Lcd_Clear();
while(1)
{
 /*ADCON0 = 0b11000001; //AN0 selected
 __delay_ms (100);
 ADCON0bits.GO_nDONE = 1;
 while(ADCON0bits.GO_nDONE);
{
 myADC =(ADRESH<<8)+ADRESL;
 mypos = ((float)myADC *5)/10.24;
}
 ADCON0 = 0b11000101; //AN1 selected
 __delay_ms (100);
 ADCON0bits.GO_nDONE = 1;
 while(ADCON0bits.GO_nDONE);
{
 myADC = (ADRESH<<8)+ADRESL;
 myneg = ((float)myADC *5)/10.24;
}
 myresult = mypos-myneg ;
 sprintf(str1, "%.1f", myresult );
 Lcd_Clear();
 Lcd_Set_Cursor (2,14);   
 Lcd_Write_String (str1);
 Lcd_Write_Char('C');*/
 
    //menu selection
   if(PORTBbits.RB1 == 0)
    {
        __delay_ms(200);
        if(PORTBbits.RB1 ==  0)
        {
            if(menu>6)
            {
                menu=2;
                
            }
            else
            {
                menu++;
                Lcd_Clear();
            }        
        }
   }
  /* Lcd_Set_Cursor(1,2);
   Lcd_Write_String("Choose Option:");*/
   
   if(PORTBbits.RB2 == 0)
   {
       __delay_ms(200);
       if(PORTBbits.RB2 == 0)
       {           
        if(up>4)
         {
            up=1;
            Lcd_Clear();
         }
          else
          {
             up++;
             Lcd_Clear();
          }
       }
   }
    if(PORTBbits.RB3 == 0)
   {
       __delay_ms(200);
       if(PORTBbits.RB3 == 0)
       {           
        if(enter>2)
         {
            enter=0;
            Lcd_Clear();
         }
          else
          {
             enter++;
             Lcd_Clear();
          }
       }
   }
     if(PORTBbits.RB4 == 0)
   {
       __delay_ms(200);
       if(PORTBbits.RB4 == 0)
       {           
        if(stop>1)
         {
            stop=0;
            Lcd_Clear();
         }
          else
          {
             stop++;
             Lcd_Clear();
          }
       }
   }
   //interupt
      if(PORTBbits.RB0 == 0)
   {
       __delay_ms(200);
       if(PORTBbits.RB0 == 0)
       { 
           inter();
       }
   }
   //Show menu option
   //if(PORTBbits.RB1 == 0)
   //{
        switch(menu)
   {
       case 2:
           Lcd_Set_Cursor(1,6);
           Lcd_Write_String("Mode");
           Lcd_Write_String(" ");
           break;
       case 3:
           Lcd_Set_Cursor(1,1);           
           Lcd_Write_String("Water Level");
           Lcd_Write_String(" ");
           break;
           
       case 4:
           Lcd_Set_Cursor(1,3);
           Lcd_Write_String("Wash Time");
           Lcd_Write_String(" ");
           break;
           
       case 5:
           Lcd_Set_Cursor(1,5);           
           Lcd_Write_String("Rinse");
           Lcd_Write_String(" ");
           break;
           
       case 6:
           Lcd_Set_Cursor(1,6);           
           Lcd_Write_String("Spin");
           Lcd_Write_String(" ");
           break;
           
       default:
           break;
           
   }
   
   
 //water level
   if(enter == 1  && menu == 3)
   {
       
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("Water Level");
       switch(up)
       {
           case 1:
                Lcd_Set_Cursor(2,7);
                Lcd_Write_String("Low"); 
                b=1;
                break;
           case 2:
               Lcd_Set_Cursor(2,3);
               Lcd_Write_String("Medium");
               b=2;
               break;
           case 3:
               Lcd_Set_Cursor(2,6);
               Lcd_Write_String("High");
               b=3;
               break;
           default:
               b=0;
               break;
       }
  
   }

   if(enter == 1  && menu == 2)
   {
       
       Lcd_Set_Cursor(1,6);
       Lcd_Write_String("Mode");
       switch(up)
       {
           case 1:
                Lcd_Set_Cursor(2,6);
                Lcd_Write_String("Normal"); 
                a=1;
                break;
           case 2:
               Lcd_Set_Cursor(2,6);
               Lcd_Write_String("Quick");
               a=2;
               break;
           case 3:
               Lcd_Set_Cursor(2,6);
               Lcd_Write_String("Rinse");
               a=3;
               break;
           default:
               a=0;
               break;
       }
  
   }
        
    //to set
   if(enter ==2)
   {
       Lcd_Clear();
       Lcd_Set_Cursor(1,7);
       Lcd_Write_String("Set");
       __delay_ms(1000);
       Lcd_Clear();
       enter = 0;
       up=1;
   
   }
   
   if(stop == 1 && a == 1)
   {
       Lcd_Clear();
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("Water In");
       PORTDbits.RD4 = 1;
       __delay_ms(1000);
       PORTDbits.RD4 = 0;
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Washing");
       PORTDbits.RD5 = 1;
       __delay_ms(3000);
        PORTDbits.RD5 = 0;
        Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Finished");
       __delay_ms(5000);
       Lcd_Clear();
       stop=0;
       
   }
   

    
        
/*    if(hour==h && minute == m && second==s)
    {
        PORTDbits.RD4 = 1;
        PORTDbits.RD5 = 0;
    }*/
 } //end while  
 } //end main


void inter()
{
    PORTDbits.RD4 = 0;
    PORTDbits.RD5 = 0;
    INTCONbits.INTF = 0;
}

/*void menu(int a) //enter menu mode
{
    if(PORTBbits.RB1 = 0)
    {
        __delay_ms(100);
        if(PORTBbits.RB1 = 0)
        {
            if(a>=6)
            {
                a=1;
            }
            else
            {
                a++;
            }        
        }
    }
}*/

