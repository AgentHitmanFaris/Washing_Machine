# Washing Machine Simulation: A-Deep-Dive

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

Ever wondered what goes on inside your washing machine? This project simulates a fully-functional washing machine using the powerful **PIC16F887 microcontroller**, brought to life in **Proteus** and coded in **MPLAB**.

## Key Features

*   🧼 **Versatile Washing Modes**: Whether it's a quick rinse or a deep clean, choose from `Normal`, `Quick Wash`, and `Custom Wash` to get the job done.
*   💧 **Customizable Cycles**: Take full control of your laundry with adjustable `Water Level`, `Wash Time`, `Rinse Time`, and `Spin Time`.
*   👆 **Intuitive Controls**: A user-friendly button interface makes it easy to navigate menus, select your desired settings, and start, pause, or stop the machine at any time.
*   🚪 **Built-in Safety**: The simulation includes a door sensor that immediately halts the machine if the door is opened, ensuring a safe and reliable operation.
*   📺 **Crystal-Clear Display**: A 16x2 LCD screen provides real-time feedback on the washing cycle, keeping you informed every step of the way.

## Tech Stack

### Hardware

*   **The Brains**: PIC16F887 Microcontroller
*   **The Display**: 16x2 Character LCD
*   **The Muscle**:
    *   DC Motor for water inlet
    *   DC Motor for drum spin
*   **The Controls**:
    *   5 Push buttons for user interaction (Menu, Select, Set, Start/Stop, Door Sensor)
*   **The Foundation**:
    *   10k Ohm Resistors
    *   100nF Capacitors
    *   Crystal Oscillator

### Software

*   **The Stage**: Proteus for simulation
*   **The Script**: MPLAB for an intuitive IDE

## The Grand Tour: Schematic Diagram

![Schematic Diagram](https://user-images.githubusercontent.com/81744011/151947136-6f0c5864-b8c2-4127-9282-88140494544e.png)

## How It All Comes Together

1.  **Power Up**: As soon as the circuit is switched on, the LCD screen comes to life, greeting you with the main menu.
2.  **Take Control**: Use the dedicated buttons to cycle through the various settings:
    *   **Mode**: `Normal`, `Quick Wash`, `Custom Wash`
    *   **Water Level**: `Low`, `Medium`, `High`
    *   **Wash Time**: `Normal`, `Medium`, `Fast`
    *   **Rinse Cycles**: `1`, `2`, or `3` times
    *   **Spin Cycles**: `1`, `2`, or `3` times
3.  **Lock It In**: Once you're happy with your selections, press the 'set' button to confirm.
4.  **Let the Magic Happen**: The washing machine will now begin its cycle, following your every command.
5.  **Safety First**: The door sensor is always on guard. If the door is opened mid-cycle, the machine will pause immediately.
6.  **Change of Plans?**: No problem! You can manually pause or stop the washing cycle at any point.

## Under the Hood: Code Snippets

### The Heartbeat: The Main Loop

```c
// The main loop of the washing machine, where all the magic happens.
while(1)
{
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
   // ... (additional button handling for selecting modes, water level, etc.)
}
```

### The Guardian: Interrupt Service Routine

```c
// This function is called whenever the door is opened, ensuring the machine stops immediately.
void inter()
{
    // Turn off the water inlet and drum spin motors
    PORTDbits.RD4 = 0;
    PORTDbits.RD5 = 0;

    // Clear the interrupt flag
    INTCONbits.INTF = 0;
}
```

### The Main Event: A Washing Cycle

```c
   if(stop == 1 && a == 1) // a==1 means normal mode has been selected
   {
       Lcd_Clear();
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String("Water In");
       PORTDbits.RD4 = 1; // Water inlet motor ON
       __delay_ms(1000);
       PORTDbits.RD4 = 0; // Water inlet motor OFF
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Washing");
       PORTDbits.RD5 = 1; // Drum spin motor ON
       __delay_ms(3000);
       PORTDbits.RD5 = 0; // Drum spin motor OFF
       Lcd_Set_Cursor(1,1);
       Lcd_Write_String(" ");
       Lcd_Write_String("Finished");
       __delay_ms(5000);
       Lcd_Clear();
       stop=0;
   }
```

## Get Your Hands Dirty

Ready to start your own laundry adventure? Here's what you'll need:

1.  **Proteus**: To open and run the simulation file (`Schematic/Washing Machine.pdsprj`).
2.  **MPLAB**: To explore and modify the source code (`Code/Main.c`).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for more details.
