/*
 * Author: Agent_Hitman_Faris
 * Refactored by: Red Team Auditor (Jules)
 *
 * Description: SOTA Washing Machine Controller for PIC16F887
 */

// --- PIC16F887 Configuration ---
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = ON
#pragma config FCMEN = ON
#pragma config LVP = OFF

#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

#define _XTAL_FREQ 8000000

// --- Pin Mapping ---
#define RS PORTCbits.RC0
#define EN PORTCbits.RC1
#define D4 PORTDbits.RD0
#define D5 PORTDbits.RD1
#define D6 PORTDbits.RD2
#define D7 PORTDbits.RD3

#define MOTOR_INLET   PORTDbits.RD4
#define MOTOR_DRUM    PORTDbits.RD5

// Active Low Buttons
#define BTN_DOOR      PORTBbits.RB0
#define BTN_MENU      PORTBbits.RB1
#define BTN_SELECT    PORTBbits.RB2
#define BTN_CONFIRM   PORTBbits.RB3
#define BTN_STARTSTOP PORTBbits.RB4

#include <xc.h>
#include <stdio.h>
#include "lcd.h"

// --- Constants & Types ---
typedef enum {
    STATE_IDLE,
    STATE_MENU_MODE,
    STATE_MENU_LEVEL,
    STATE_FILLING,
    STATE_WASHING,
    STATE_RINSING,
    STATE_SPINNING,
    STATE_DRAINING,
    STATE_COMPLETE,
    STATE_PAUSED,
    STATE_ERROR
} MachineState;

typedef enum {
    MODE_NORMAL,
    MODE_QUICK,
    MODE_HEAVY
} WashMode;

typedef struct {
    uint8_t wash_time_sec;
    uint8_t rinse_count;
    uint8_t spin_time_sec;
    char *name;
} WashProfile;

// --- Globals ---
volatile MachineState current_state = STATE_IDLE;
volatile MachineState previous_state = STATE_IDLE;
volatile uint8_t door_open_detected = 0;

WashMode selected_mode = MODE_NORMAL;
uint8_t selected_water_level = 2; // 1=Low, 2=Med, 3=High

// SOTA Profiles
const WashProfile profiles[3] = {
    {10, 2, 5, "Normal"}, // Reduced times for simulation
    {5,  1, 3, "Quick "},
    {15, 3, 8, "Heavy "}
};

// --- Function Prototypes ---
void System_Init(void);
void Update_Display(void);
void Handle_Inputs(void);
void Run_FSM(void);
void Error_Handler(void);
void __interrupt() isr(void);

// --- Main Application ---
void main(void) {
    System_Init();

    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("System Boot...");
    __delay_ms(1000);
    Lcd_Clear();

    current_state = STATE_IDLE;

    while(1) {
        Handle_Inputs();
        Run_FSM();
        // Small delay to prevent CPU hogging in simulation and allow LCD to stabilize
        __delay_ms(50);
    }
}

void System_Init(void) {
    // Port Configuration
    TRISB = 0xFF; // PORTB as Input (Buttons)
    TRISD = 0x00; // PORTD as Output (LCD + Motors)
    TRISC = 0x00; // PORTC as Output (LCD Control)

    PORTD = 0x00;
    PORTC = 0x00;

    // ADC Off
    ANSEL = 0x00;
    ANSELH = 0x00;

    // Interrupt Configuration
    INTCONbits.GIE = 1;  // Global Interrupt Enable
    INTCONbits.INTE = 1; // External Interrupt Enable (RB0/Door)
    OPTION_REGbits.INTEDG = 0; // Falling edge for RB0

    Lcd_Init();
}

void __interrupt() isr(void) {
    if (INTCONbits.INTF) {
        // Door Sensor Triggered
        MOTOR_INLET = 0;
        MOTOR_DRUM = 0;

        if (current_state != STATE_IDLE && current_state != STATE_ERROR) {
            previous_state = current_state;
            current_state = STATE_ERROR;
            door_open_detected = 1;
        }

        INTCONbits.INTF = 0;
    }
}

void Handle_Inputs(void) {
    // Debounce Delay inside button checks if needed, but keeping it simple for FSM

    if (current_state == STATE_ERROR) {
        if (BTN_STARTSTOP == 0) {
            __delay_ms(200);
            if (door_open_detected) {
                door_open_detected = 0;
                current_state = STATE_IDLE; // Reset to IDLE
            }
        }
        return;
    }

    if (current_state == STATE_IDLE || current_state == STATE_MENU_MODE || current_state == STATE_MENU_LEVEL) {
        if (BTN_MENU == 0) {
            __delay_ms(200);
            if (current_state == STATE_IDLE) current_state = STATE_MENU_MODE;
            else if (current_state == STATE_MENU_MODE) current_state = STATE_MENU_LEVEL;
            else current_state = STATE_IDLE;
            Lcd_Clear();
        }

        if (BTN_SELECT == 0) {
            __delay_ms(200);
            if (current_state == STATE_MENU_MODE) {
                selected_mode++;
                if (selected_mode > MODE_HEAVY) selected_mode = MODE_NORMAL;
            } else if (current_state == STATE_MENU_LEVEL) {
                selected_water_level++;
                if (selected_water_level > 3) selected_water_level = 1;
            }
        }
    }

    if (BTN_CONFIRM == 0 && (current_state == STATE_MENU_MODE || current_state == STATE_MENU_LEVEL)) {
        __delay_ms(200);
        current_state = STATE_IDLE;
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Settings Saved");
        __delay_ms(1000);
        Lcd_Clear();
    }

    if (BTN_STARTSTOP == 0) {
        __delay_ms(200);
        if (current_state == STATE_IDLE) {
            current_state = STATE_FILLING;
            Lcd_Clear();
        } else if (current_state == STATE_PAUSED) {
            current_state = previous_state;
        } else if (current_state > STATE_IDLE && current_state < STATE_COMPLETE) {
            previous_state = current_state;
            current_state = STATE_PAUSED;
        }
    }
}

// Helper for non-blocking delays with State checks
void Wait_With_Check(uint16_t milliseconds) {
    uint16_t i;
    for(i=0; i<milliseconds/10; i++) {
        __delay_ms(10);

        // Poll for Pause (Start/Stop Button)
        if (BTN_STARTSTOP == 0) {
             __delay_ms(20); // Debounce
             if (BTN_STARTSTOP == 0) {
                 if (current_state != STATE_ERROR && current_state != STATE_PAUSED) {
                     previous_state = current_state;
                     current_state = STATE_PAUSED;
                     // Wait for release to avoid immediate toggle in Handle_Inputs
                     while(BTN_STARTSTOP == 0);
                 }
                 return;
             }
        }

        if (current_state == STATE_ERROR) return;
    }
}

void Run_FSM(void) {
    char buffer[17];

    switch (current_state) {
        case STATE_IDLE:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Ready: ");
            Lcd_Write_String(profiles[selected_mode].name);
            Lcd_Set_Cursor(2,1);
            sprintf(buffer, "Lvl:%d Press Start", selected_water_level);
            Lcd_Write_String(buffer);
            break;

        case STATE_MENU_MODE:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Select Mode:");
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String(profiles[selected_mode].name);
            break;

        case STATE_MENU_LEVEL:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Water Level:");
            Lcd_Set_Cursor(2,1);
            if(selected_water_level==1) Lcd_Write_String("Low");
            else if(selected_water_level==2) Lcd_Write_String("Medium");
            else Lcd_Write_String("High");
            break;

        case STATE_FILLING:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Filling Water...");
            MOTOR_INLET = 1;
            Wait_With_Check(2000); // Simulate fill time
            MOTOR_INLET = 0;
            if (current_state != STATE_ERROR && current_state != STATE_PAUSED)
                current_state = STATE_WASHING;
            break;

        case STATE_WASHING:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Washing...");
            MOTOR_DRUM = 1;
            Wait_With_Check(profiles[selected_mode].wash_time_sec * 100); // Scale for sim
            MOTOR_DRUM = 0;
            if (current_state != STATE_ERROR && current_state != STATE_PAUSED)
                current_state = STATE_RINSING;
            break;

        case STATE_RINSING:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Rinsing...");
            MOTOR_INLET = 1;
            Wait_With_Check(1000);
            MOTOR_INLET = 0;
            MOTOR_DRUM = 1;
            Wait_With_Check(1000);
            MOTOR_DRUM = 0;
            if (current_state != STATE_ERROR && current_state != STATE_PAUSED)
                current_state = STATE_SPINNING;
            break;

        case STATE_SPINNING:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Spinning...");
            MOTOR_DRUM = 1;
            Wait_With_Check(profiles[selected_mode].spin_time_sec * 100);
            MOTOR_DRUM = 0;
            if (current_state != STATE_ERROR && current_state != STATE_PAUSED)
                current_state = STATE_COMPLETE;
            break;

        case STATE_COMPLETE:
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("Cycle Complete");
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String("Remove Clothes");
            __delay_ms(3000);
            current_state = STATE_IDLE;
            Lcd_Clear();
            break;

        case STATE_PAUSED:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("PAUSED");
            MOTOR_INLET = 0;
            MOTOR_DRUM = 0;
            break;

        case STATE_ERROR:
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String("ERROR: DOOR OPEN");
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String("Close & Restart");
            break;
    }
}
