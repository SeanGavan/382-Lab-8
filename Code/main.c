#include <msp430.h>

void moveForward(void);		// declaring methods for later
void leftTurn(void);
void rightTurn(void);
void getCenter(void);
void getLeft(void);
void isCenterClose(void);
void isLeftClose(void);

unsigned short centerSample[16];
unsigned short leftSample[16];
unsigned int t = 0;
unsigned int left_index = 0;
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
	centerSample[center_index] = ADC10MEM;
}

void isCenterClose(void){
	while(centerSample[center_index]>THRESHOLDCENTER){
		getLeft();
		isLeftClose();
		centerSample[center_index] = 0;
	}
	moveForward();
}

void getLeft(void){
	left_index = 0;
	ADC10CTL0 = 0;											// Turn off ADC
	ADC10CTL1 = INCH_5;						// Use channel A5
	ADC10AE0 = BIT5;										// Enable P1.5 as ADC input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// ADC conversion
	ADC10CTL0 &= ~ENC;
	while(ADC10CTL1 & ADC10BUSY);
	leftSample[left_index] = ADC10MEM;
}

void isLeftClose(void){
	while(leftSample[left_index]<THRESHOLDLEFT){
		leftTurn();
		leftSample[t] = 0xFFFF;
		return;
	}
	while(leftSample[left_index]>THRESHOLDLEFT){
		rightTurn();
		leftSample[t] = 0;
		return;
	}
}

void stop(void){
	t = 0;
	while(t<0xFFFF){
		P2OUT &= ~BIT0;
		P2OUT &= ~BIT3;
		t++;
	}
	P2OUT |= BIT0;
	P2OUT |= BIT3;
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

	int t = 0;
	while (t<0x58FF){
		P2OUT &= ~BIT1;
		P2OUT &= ~BIT5;
		t++;
	}
}

void rightTurn(void) {

	int t = 0;
	while (t<0x5CFF){
		P2OUT |= BIT1;
		P2OUT |= BIT5;
		t++;
	}
}
