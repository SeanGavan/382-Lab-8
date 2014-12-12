#include <msp430.h>

void moveForward(void);		// declaring methods for later
void leftTurn(void);
void rightTurn(void);
void getCenter(void);
void getLeft(void);
void isCenterClose(void);
void isLeftClose(void);
void stop(void);

unsigned short centerSample[16];
unsigned short leftSample[16];
unsigned int t = 0;

int THRESHOLDLEFT = 0x3B0;		// Threshold voltage for wall prescence
int THRESHOLDCENTER = 0x3D0;

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
	t = 0;
	ADC10CTL0 = 0;											// Turn off ADC
	ADC10CTL1 = INCH_2;						// Use channel A2
	ADC10AE0 = BIT2;										// Enable P1.2 as ADC input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// ADC conversion
	ADC10CTL0 &= ~ENC;
	while(ADC10CTL1 & ADC10BUSY);
	centerSample[t] = ADC10MEM;
}

void isCenterClose(void){
	while(centerSample[t]<THRESHOLDCENTER){
		getLeft();
		isLeftClose();
		centerSample[t] = 0xFFFF;
	}
//	if(centerSample[t]<THRESHOLDCENTER){
//		stop();
//		getLeft();
//		isLeftClose();
//	}
	moveForward();
}

void getLeft(void){
	t = 0;
	ADC10CTL0 = 0;											// Turn off ADC
	ADC10CTL1 = INCH_1;						// Use channel A1
	ADC10AE0 = BIT1;										// Enable P1.1 as ADC input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// ADC conversion
	ADC10CTL0 &= ~ENC;
	while(ADC10CTL1 & ADC10BUSY);
	leftSample[t] = ADC10MEM;
}

void isLeftClose(void){
	while(leftSample[t]>THRESHOLDLEFT){
		leftTurn();
		leftSample[t] = 0;
		return;
	}
	while(leftSample[t]<THRESHOLDLEFT){
		rightTurn();
		leftSample[t] = 0xFFFF;
		return;
	}
//	if(leftSample[t]>THRESHOLDLEFT){
//		leftTurn();
//	} else {
//		rightTurn();
//	}
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
	TA1CCR1 = 0x0050;
	while (t<0xFFFF){
		P2OUT &= ~BIT1;
		P2OUT &= ~BIT5;
		t++;
	}
	TA1CCR1 = 0x0020;
}

void rightTurn(void) {

	int t = 0;
    TA1CCR2 = 0x0030;
	while (t<0xFFFF){
		P2OUT |= BIT1;
		P2OUT |= BIT5;
		t++;
	}
    TA1CCR2 = 0x0020;
}
