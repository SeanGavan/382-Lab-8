# 382-Lab-8
## Objectives
The goal of this lab was to combine labs 6 and 7 to have the robot navigate a maze. To do so, I implemented a "follow-the-left-wall" method of navigation.  
## Preliminary design
![alt text](https://raw.githubusercontent.com/SeanGavan/382-Lab-8/master/Images/PreLab.PNG "Prelab")
## Software flow chart
1) Establish arrays and indexes
2) Establish constants for thresholds
3) Initialize the motor system and timers
4) Poll center sensor to check for a wall
5) If no wall, then move forward
6) IF there is, check the left sensor
7) If there is a wall on the left, turn right
8) Otherwise, turn left (follow the left wall)
## Hardware schematic
![alt text](https://raw.githubusercontent.com/SeanGavan/382-Lab-8/master/Images/Schematic.PNG "MSP430")
![alt text](https://raw.githubusercontent.com/SeanGavan/382-Lab-6/master/Images/Hardware.png "Hardware")
## Well-formatted code
```
/**
 * Sean Gavan
 * 12 Dec. 14
 * Lab 8 -- Robot Maze
 * This lab had us combine labs 6 and 7 (using the motors and detecting walls with the IR sensors)
 * to navigate our robot through a maze. This code uses a "follow the left wall" system to do so.
 * Documentation: C2C Kiernan allowed me to look at his code in order to get a good organization style.
 * I also used Dr. Coulston's coding template for the ADC10 system.
 */
#include <msp430.h>

void moveForward(void);		// declaring methods for later
void leftTurn(void);
void rightTurn(void);
void getCenter(void);
void getLeft(void);
void isCenterClose(void);
void isLeftClose(void);

unsigned short centerSample[16];	// arrays to store ADC10MEM
unsigned short leftSample[16];
unsigned int t = 0;					// generic timer variable for movement methods
unsigned int left_index = 0;		// array index counter
unsigned int center_index = 0;

int THRESHOLDLEFT = 0x170;		// Threshold voltage for wall prescence
int THRESHOLDCENTER = 0x120;	// Lights on

//int THRESHOLDLEFT = 0xFF;		// Lights off
//int THRESHOLDCENTER = 0x100;

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;				  // Stop watchdog timer

    P2DIR |= BIT2;							// P2.2 is associated with TA1CCR1
    P2SEL |= BIT2;							// P2.2 is associated with TA1CCTL1

    P2DIR |= BIT4;							// P2.4 is associated with TA1CCR2
    P2SEL |= BIT4;							// P2.4 is associated with TA1CCTL2

    P2DIR |= BIT0;							// Enable for L motor
    P2OUT |= BIT0;

    P2DIR |= BIT1;						    // Set output pin for L motor
    P2OUT |= BIT1;

    P2DIR |= BIT3;							// Enable for R motor
    P2OUT |= BIT3;

    P2DIR |= BIT5;							// Set output pin for R motor
    P2OUT |= BIT5;

	TA1CTL = ID_3 | TASSEL_2 | MC_1;		// Use 1:8 presclar off MCLK
    TA1CCR0 = 0x0100;						// set signal period

    TA1CCR1 = 0x0020;
    TA1CCTL1 = OUTMOD_7;					// set TACCTL1 to Reset / Set mode

    TA1CCR2 = 0x0020;
    TA1CCTL2 = OUTMOD_3;					// set TACCTL1 to Reset / Set mode

    while (1) {
    	getCenter();
    	isCenterClose();
    } // end loop
} // end main

void getCenter(void){
	center_index = 0;
	ADC10CTL0 = 0;											// Turn off ADC
	ADC10CTL1 = INCH_4;						// Use channel A4
	ADC10AE0 = BIT4;										// Enable P1.4 as ADC input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// ADC conversion
	ADC10CTL0 &= ~ENC;
	while(ADC10CTL1 & ADC10BUSY);
	centerSample[center_index] = ADC10MEM;					// Store IR receiver reading
}

void isCenterClose(void){
	while(centerSample[center_index]>THRESHOLDCENTER){		// If there's a wall in front...
		getLeft();											// Check for a wall on the left
		isLeftClose();										// Is there one?
		centerSample[center_index] = 0;						// Sets the array to exit the while loop
	}
	moveForward();											// Move forward either after turning or if not necessary
}

void getLeft(void){
	left_index = 0;											// Reset array
	ADC10CTL0 = 0;											// Turn off ADC
	ADC10CTL1 = INCH_5;										// Use channel A5
	ADC10AE0 = BIT5;										// Enable P1.5 as ADC input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// ADC conversion
	ADC10CTL0 &= ~ENC;										// Stop conversion
	while(ADC10CTL1 & ADC10BUSY);
	leftSample[left_index] = ADC10MEM;						// Store IR reading
}

void isLeftClose(void){
	while(leftSample[left_index]<THRESHOLDLEFT){			// Compare IR reading to check for a wall
		leftTurn();											// No wall on the left -- turn that way
		leftSample[left_index] = 0xFFFF;					// Exit while
	}
	while(leftSample[left_index]>THRESHOLDLEFT){			// Wall to the left
		rightTurn();										// Turn right
		leftSample[left_index] = 0;							// Exit
	}
}

void moveForward(void) {

	int t = 0;				// sets time for moving forward
	while (t<0xFFFF){
	    P2OUT |= BIT1;		// L motor CCW
	    P2OUT &= ~BIT5;		// R motor CW
		t++;
	}
}

void leftTurn(void) {

	int t = 0;				// another timer
	while (t<0x58FF){		// ~90 degree turn
		P2OUT &= ~BIT1;		// L motor CW
		P2OUT &= ~BIT5;		// R motor CW
		t++;
	}
}

void rightTurn(void) {

	int t = 0;				// timers
	while (t<0x5CFF){		// ~90 degrees
		P2OUT |= BIT1;		// L = CCW
		P2OUT |= BIT5;		// R = CCW
		t++;
	}
}
```
## Debugging
My original idea had one large while loop with multiple embedded loops. After finding that the ADC10 system may have been incorrectly receiving, I decided that I needed a new approach. This meant that I would separate the sampling and conversions into their own respective functions to allow the ADC10 time to process everything. This also allowed for cleaner code as well.  
Also, I had been using the P1.1 and P1.2 pins incorrectly. They were used in the motor lab for the timers, but used for ADC input in the IR lab. This meant that I needed to redefine my pins so that nothing would be conflicting. I did so by changing the ADC input to P1.4 and P1.5 since they were not being used.  
The sensors would give very large voltages, this was because the sensors were being pointed upward toward the bottom of the metal surface of the circuit board. This was fixed by simply correcting them when needed since they would also be shifted when a wall was hit during testing
## Testing methodology
I would send the bot through the maze to see how each turn specific timing delay would work. I wanted the turns to be as close to 90 degrees as possible so that I could cut down on time that it would take to go through the maze.  
The lighting in the room also played a part in how the sensors worked, and turning one light off resulted in less voltage being read by sensor. This meant that I needed to have two sets of thresholds for either scenario. I found these using the same method as Lab 7; finding the value when a wall was not close (baseline), and when one was close. 
## Observations
This lab served as a culmination of the semester's work. Not only did it combine the work of Lab 6 and Lab 7, but I also needed to understand how the MSP430 pins worked, and how to wire the circuit for the functionality I wanted. It also showed me how vital it is to keep track of what pins are being used and how. 
## Documentation
I used C2C Kiernan's code to guide my organization, and also Dr. Coulston's code for a template on using the ADC10 system.
