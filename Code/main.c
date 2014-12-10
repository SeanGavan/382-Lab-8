#include <msp430.h>

void moveForward(void);		// declaring methods for later
void moveBackward(void);
void smallLeft(void);
void smallRight(void);
void bigLeft(void);
void bigRight(void);
void enable(void);

int THRESHOLDLEFT = 0x3B0;		// Threshold voltage for wall prescence
int THRESHOLDCENTER = 0x3D0;

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;				  // Stop watchdog timer
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE; 		  // 64 ADC clks to combat loading, ADC10ON
    ADC10AE0 |= BIT1 + BIT2 + BIT4;           // P1.1, P1.2, P1.4 to ADC mode (A1,A2,A4)
    ADC10CTL1 |= ADC10SSEL1|ADC10SSEL0;       // Select SMCLK

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

    } // end loop
} // end main

void enable(void) {			// sets the enables for the motors
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

    P2OUT &= ~BIT0;			// stop motors to show next step is working
    P2OUT &= ~BIT3;

}

void moveBackward(void) {

    TA1CCR1 = 0x0100;		// set motors to be the same speed
    TA1CCR2 = 0x0100;

	int t = 0;
	while (t<0xFFFF){
	    P2OUT &= ~BIT1;		// L motor CW
	    P2OUT |= BIT5;		// R motor CCW
		t++;
	}

    P2OUT &= ~BIT0;
    P2OUT &= ~BIT3;

    TA1CCR1 = 0x0020;		// set the motors back to normal speed
    TA1CCR2 = 0x0020;

}

void smallLeft(void) {

	int t = 0;
	while (t<0xAFFF){		// less time for <45 degree angle
		P2OUT &= ~BIT1;
		P2OUT &= ~BIT5;
		t++;
	}

    P2OUT &= ~BIT0;
    P2OUT &= ~BIT3;

}

void smallRight(void) {


	int t = 0;
	while (t<0xAFFF){
		P2OUT |= BIT1;
		P2OUT |= BIT5;
		t++;
	}

    P2OUT &= ~BIT0;
    P2OUT &= ~BIT3;

}

void bigLeft(void) {

	int t = 0;
	while (t<0xFFFF){		// more time for a >45 degree angle
		P2OUT &= ~BIT1;
		P2OUT &= ~BIT5;
		t++;
	}

    P2OUT &= ~BIT0;
    P2OUT &= ~BIT3;
}

void bigRight(void) {

	int t = 0;
	while (t<0xFFFF){
		P2OUT |= BIT1;
		P2OUT |= BIT5;
		t++;
	}

    P2OUT &= ~BIT0;
    P2OUT &= ~BIT3;
}
