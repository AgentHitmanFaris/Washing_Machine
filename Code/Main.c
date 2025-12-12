/*
 * Author: Agent_Hitman_Faris
 *
 * Created with love
 */

// --- PIC16F887 Configuration ---
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

// --- Pin Definitions ---
// Renamed to match lcd.h requirements
#define RS PORTCbits.RC0
#define EN PORTCbits.RC1
#define D4 PORTDbits.RD0
#define D5 PORTDbits.RD1
#define D6 PORTDbits.RD2
#define D7 PORTDbits.RD3

#define WATER_INLET_MOTOR PORTDbits.RD4
#define DRUM_SPIN_MOTOR PORTDbits.RD5

#define DOOR_SENSOR_BUTTON PORTBbits.RB0
#define MENU_BUTTON PORTBbits.RB1
#define SELECT_BUTTON PORTBbits.RB2
#define CONFIRM_BUTTON PORTBbits.RB3
#define START_STOP_BUTTON PORTBbits.RB4

#include <stdlib.h>
#include <stdio.h> //header file for sprintf
#include <xc.h>
#include "lcd.h"

// --- Enums for better readability ---
enum MenuOption {
    MODE = 2,
    WATER_LEVEL = 3,
    WASH_TIME = 4,
    RINSE = 5,
    SPIN = 6
};

enum MachineState {
    STOPPED,
    RUNNING
};

// --- Global Variables ---
int menu_selection = 1;
int confirm_selection = 0;
volatile enum MachineState machine_state = STOPPED;
enum MenuOption current_menu = MODE;
int selected_mode = 0;
int selected_water_level = 0;
int selected_wash_time = 0;
int selected_rinse_time = 0;
int selected_spin_time = 0;

int display_dirty = 1; // Optimization: Only update LCD when needed


// --- Function Prototypes ---
void initialize_system();
void handle_button_presses();
void update_display();
void run_washing_cycle();

void main()
{
    initialize_system();

    while(1)
    {
        handle_button_presses();
        update_display();
        run_washing_cycle();
    }
}

/**
 * @brief Initializes all the necessary hardware components for the washing machine.
 */
void initialize_system()
{
    // --- Port Configuration ---
    ANSEL = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    ANSELH = 0x00;
    TRISB = 0xFF; // Set PORTB as input for buttons
    OSCCON = 0x71; // Configure internal oscillator
    PORTA = 0x00;
    PORTB = 0xFF;
    PORTC = 0x00;
    PORTD = 0x00;

    // --- Motor Initialization ---
    WATER_INLET_MOTOR = 0;
    DRUM_SPIN_MOTOR = 0;

    // --- Interrupt Configuration ---
    INTCONbits.INTE =1;
    INTCONbits.GIE =1;
    OPTION_REGbits.INTEDG = 0;

    // --- LCD Initialization ---
    Lcd_Init();
    Lcd_Clear();
}

/**
 * @brief Handles all the button presses from the user.
 */
void handle_button_presses()
{
    // --- Menu Button ---
    if(MENU_BUTTON == 0)
    {
        __delay_ms(200);
        if(MENU_BUTTON == 0)
        {
            if(current_menu >= SPIN)
            {
                current_menu = MODE;
            }
            else
            {
                current_menu++;
            }
            Lcd_Clear();
            display_dirty = 1;
        }
    }

    // --- Select Button ---
    if(SELECT_BUTTON == 0)
    {
       __delay_ms(200);
       if(SELECT_BUTTON == 0)
       {
          if(menu_selection > 3) // Assuming max 3 options per menu
          {
             menu_selection = 1;
          }
          else
          {
             menu_selection++;
          }
          Lcd_Clear();
          display_dirty = 1;
       }
    }

    // --- Confirm Button ---
    if(CONFIRM_BUTTON == 0)
    {
       __delay_ms(200);
       if(CONFIRM_BUTTON == 0)
       {
          if(confirm_selection > 1)
          {
             confirm_selection = 0;
          }
          else
          {
             confirm_selection++;
          }
          Lcd_Clear();
          display_dirty = 1;
       }
    }

    // --- Start/Stop Button ---
    if(START_STOP_BUTTON == 0)
    {
       __delay_ms(200);
       if(START_STOP_BUTTON == 0)
       {
          if(machine_state == STOPPED)
          {
             machine_state = RUNNING;
          }
          else
          {
             machine_state = STOPPED;
          }
          Lcd_Clear();
          display_dirty = 1;
       }
    }

    // --- Door Sensor Button ---
    // Handled by Interrupt Service Routine (ISR) for immediate safety response
}

/**
 * @brief Updates the LCD display based on the current menu and selections.
 */
void update_display()
{
    if (!display_dirty) {
        return;
    }

    // Reset dirty flag, but might set it again if we do an auto-transition
    display_dirty = 0;

    // --- Display Menu ---
    switch(current_menu)
    {
       case MODE:
           Lcd_Set_Cursor(1,6);
           Lcd_Write_String("Mode");
           break;
       case WATER_LEVEL:
           Lcd_Set_Cursor(1,1);
           Lcd_Write_String("Water Level");
           break;
       case WASH_TIME:
           Lcd_Set_Cursor(1,3);
           Lcd_Write_String("Wash Time");
           break;
       case RINSE:
           Lcd_Set_Cursor(1,5);
           Lcd_Write_String("Rinse");
           break;
       case SPIN:
           Lcd_Set_Cursor(1,6);
           Lcd_Write_String("Spin");
           break;
    }

    // --- Display Submenu Options ---
    if(confirm_selection == 1)
    {
       if(current_menu == WATER_LEVEL)
       {
           Lcd_Set_Cursor(1,1);
           Lcd_Write_String("Water Level");
           switch(menu_selection)
           {
               case 1:
                    Lcd_Set_Cursor(2,7);
                    Lcd_Write_String("Low");
                    selected_water_level=1;
                    break;
               case 2:
                   Lcd_Set_Cursor(2,3);
                   Lcd_Write_String("Medium");
                   selected_water_level=2;
                   break;
               case 3:
                   Lcd_Set_Cursor(2,6);
                   Lcd_Write_String("High");
                   selected_water_level=3;
                   break;
           }
       }

       if(current_menu == MODE)
       {
           Lcd_Set_Cursor(1,6);
           Lcd_Write_String("Mode");
           switch(menu_selection)
           {
               case 1:
                    Lcd_Set_Cursor(2,6);
                    Lcd_Write_String("Normal");
                    selected_mode=1;
                    break;
               case 2:
                   Lcd_Set_Cursor(2,6);
                   Lcd_Write_String("Quick");
                   selected_mode=2;
                   break;
               case 3:
                   Lcd_Set_Cursor(2,6);
                   Lcd_Write_String("Rinse");
                   selected_mode=3;
                   break;
           }
       }
    }

    // --- Confirm Selection ---
    if(confirm_selection == 2)
    {
       Lcd_Clear();
       Lcd_Set_Cursor(1,7);
       Lcd_Write_String("Set");
       __delay_ms(1000);
       Lcd_Clear();
       confirm_selection = 0;
       menu_selection = 1;
       // We just reset the selection, so we need to redraw the main menu immediately/next loop.
       display_dirty = 1;
    }
}

/**
 * @brief Runs the washing cycle based on the user's selections.
 */
void run_washing_cycle()
{
    if(machine_state == RUNNING && selected_mode == 1) // Normal Mode
    {
       Lcd_Clear();
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("Water In");
       WATER_INLET_MOTOR = 1;
       __delay_ms(1000);
       WATER_INLET_MOTOR = 0;

       if (machine_state != RUNNING) return; // Safety check

       Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Washing");
       DRUM_SPIN_MOTOR = 1;
       __delay_ms(3000);
       DRUM_SPIN_MOTOR = 0;

       if (machine_state != RUNNING) return; // Safety check

       Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Finished");
       __delay_ms(5000);
       Lcd_Clear();
       machine_state = STOPPED;
       display_dirty = 1; // Restore menu
    }
}

/**
 * @brief Interrupt service routine for the door sensor (Safety Stop).
 */
void __interrupt() isr(void)
{
    if (INTCONbits.INTF)
    {
        // Emergency Stop
        WATER_INLET_MOTOR = 0;
        DRUM_SPIN_MOTOR = 0;
        machine_state = STOPPED;
        display_dirty = 1; // Update display on next loop
        INTCONbits.INTF = 0; // Clear interrupt flag
    }
}
