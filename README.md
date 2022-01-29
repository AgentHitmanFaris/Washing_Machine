# Washing_Machine
Washing Machine project using PIC16F887 in Proteus and MPLAB

////////////////////Schematic Diagram////////////////////

![image](https://user-images.githubusercontent.com/81744011/151532789-c43ad0b8-f2cf-4763-83c7-7a57b61aadb4.png)

////////////////////Initial Idea////////////////////

i) Port Used
1. RB0 - To check whether the door open or closed using internal inttertupt in PIC
2. RB1 - To select menu (mode, wash time, water level, rinsing times, spin time)
3. RB2 - To select the mode menu (example: to choose diffrent water level such as low, medium, high)
4. RB3 - To confirm the selction menu/mode menu
5. RB4 - To strat/pause/stop the process
6. RC0, RC1, RD0, RD1, RD2, RD3 - Use for LCD pin in 4-bit
7. RD4 - Motor for water inlet
8. RD5 - Motor for drum spin
9. RE3 - Master clear button

ii) How it's work
1. When the circuit switch on, the LCD will prompt menu initally as mode.
2. The menu consits of:

    a) Mode - Have diffrent selection; Normal, Quick Wash and Custom Wash
    
    b) Water Level - Choose diffrent water level; Low, Medium, High
    
    c) Wash Time - Choose how long to soak the clothes; Normal, Medium, Fast
    
    d) Rinse Time - Choose to rinse for how many times; 1, 2, 3 times
    
    e) Spin Time - Choose to spin for how many timer; 1, 2, 3 times
    
3) After select the menu, press button 'set' to confirm the selection and the LCD will promp the confim selection based on the user has set earlier.
4) The machine will continue the process until finished.
5) The door check button will always been pressed to stimulate the door is closed if the button are not press and the machine already works (not during the mode selection process) all the activity will imidiately stop working
6) The user also can override the working process by press the start/pause/stop button

////////////////////The Code////////////////////

-WILL UPDATE SOON-
   

