#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "usb.h"
#include "oc.h"
#include <stdio.h>
#include <time.h>
#define SET_TIMER 0
#define GET_STATE 1
#define SET_THRESHOLD 2
#define GET_VOLTAGE 3
#define SET_DUTY 4
#define GET_STEP_COUNT 5
#define SET_SPEED 6


#define MAX_INT 65535
#define HALF_INT (.5 * 65536)

// Name motor controller pins
_PIN * outPin = 0;
_PIN * redPin = 0;
_PIN * bluePin = 0;
_PIN * greenPin = 0;
_PIN * inPin0 = 0;
_PIN * inPin1 = 0;
_PIN * inPin2 = 0;
_PIN * testPin = 0;
_PIN * stepPin = 0;
_PIN * dirPin = 0;
_PIN * nSleepPin = 0;

// Global variables;
float period = 0;
int dir = 0;
uint16_t destination;
int step = 0;
uint16_t stepCount = HALF_INT;
int change = 0;
int motorOn = 0;
int testVar = 0;

uint16_t Vin0 = 0;
uint16_t VatTime0 = 0;
uint16_t Vin1 = 0;
uint16_t VatTime1 = 0;
uint16_t Vin2 = 0;
uint16_t VatTime2 = 0;
int touching0 = 0;
int touching1 = 0;
int touching2 = 0;
uint16_t threshold = 0;
int petal = 0;

int greenOn = 0;
int redOn = 0;
int blueOn = 0;
int greenChange = 0;
int redChange = 0;
int blueChange = 0;
uint16_t greenDuty = 0;
uint16_t redDuty = 0;
uint16_t blueDuty = 0;


void timer_serviceInterrupt(_TIMER *self) {
	timer_lower(self);		//lower the flag
	timer_stop(self);

	testVar = !testVar;
	pin_write(testPin, testVar);
	
	if (stepCount != destination){
		if (motorOn == 1){
			stepCount += step;
			motorOn = 0;
			pin_write(stepPin, 0);
		} else if (motorOn == 0){
			motorOn = 1;
			if (stepCount > destination){
				step = -1; 
				dir = 0;
			} else if (stepCount < destination){
				step = 1;
				dir = 1;
			}
			pin_write(dirPin, dir); 
			pin_write(stepPin, 1);
		}
	timer_start(self);
	}
	
}

void __attribute__((interrupt, auto_psv)) _CNInterrupt(void) {
	IFS1bits.CNIF = 0;		//lower the flag
} 

void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void) {
	IFS0bits.OC1IF = 0; 		//reset oc flag

	if (petal == 0){
		Vin0 = pin_read(inPin0);
		if (Vin0 != 0){
			VatTime0 = Vin0;
			if (Vin0 > threshold){
				touching0 = 0;
			} else {
				touching0 = 10;
			}
		}
		petal = 1;
	}
	else if (petal == 1){
		Vin1 = pin_read(inPin1);
		if (Vin1 != 0){
			VatTime1 = Vin1;
			if (Vin1 > threshold){
				touching1 = 1;
			} else {
				touching1 = 11;
			}
		}
		petal = 2;
	}
	else if (petal == 2){
		Vin2 = pin_read(inPin2);
		if (Vin2 != 0){
			VatTime2 = Vin2;
			if (Vin2 > threshold){
				touching2 = 2;
			} else {
				touching2 = 12;
			}
		}
		petal = 0;
	}
	//timer_start(&timer1);
	}

void VendorRequests(void) {
	WORD temp;

	switch (USB_setup.bRequest) {
		case GET_VOLTAGE:
			temp.w = VatTime0;
			BD[EP0IN].address[0] = VatTime0/256;
			BD[EP0IN].address[1] = VatTime1/256;
			BD[EP0IN].address[2] = VatTime2/256;
			BD[EP0IN].bytecount = 3;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break; 
		case GET_STATE:
			BD[EP0IN].address[0] = touching0;
			BD[EP0IN].address[1] = touching1;
			BD[EP0IN].address[2] = touching2;
			BD[EP0IN].bytecount = 3;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case SET_THRESHOLD:	
			threshold = USB_setup.wValue.w;
			BD[EP0IN].bytecount = 0;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case SET_DUTY:	
			destination = USB_setup.wValue.w;
			change = 1;
			testVar = !testVar;
			BD[EP0IN].bytecount = 0;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case GET_STEP_COUNT:
			temp.w = stepCount;
           		BD[EP0IN].address[0] = temp.b[0];
            		BD[EP0IN].address[1] = temp.b[1];
            		BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 0
            		BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            		break;  
		case SET_SPEED:	
			timer_setFreq(&timer1, USB_setup.wValue.w);
			BD[EP0IN].bytecount = 0;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;      
		default:
			USB_error_flags |= 0x01;	// set Request Error Flag
	}
}

void VendorRequestsIn(void) {
	switch (USB_request.setup.bRequest) {
		default:
			USB_error_flags |= 0x01;					// set Request Error Flag
	}
}

void VendorRequestsOut(void) {
	switch (USB_request.setup.bRequest) {
		default:
			USB_error_flags |= 0x01;					// set Request Error Flag
	}
}

int16_t main(void) {
	init_clock();
	init_timer();
	init_pin();
	init_oc();

	InitUSB();							  // initialize the USB registers and serial interface engine
	while (USB_USWSTAT!=CONFIG_STATE) {	 // while the peripheral is not configured...
		ServiceUSB();					   // ...service USB requests
	}

	// Configure Interrupts on the pic
	IEC1bits.CNIE = 1;
	CNEN1bits.CN2IE = 1;
	IFS1bits.CNIF = 0;

	IEC0bits.OC1IE = 1;
	IFS0bits.OC1IF = 0;
	timer_enableInterrupt(&timer1);
	timer_lower(&timer1);


	// Configure Pins
	inPin0 = &A[0];	
	pin_analogIn(inPin0);
	inPin1 = &A[1];	
	pin_analogIn(inPin1);
	inPin2 = &A[2];	
	pin_analogIn(inPin2);

	outPin = &D[2];
	pin_digitalOut(outPin);
	oc_pwm(&oc1, outPin, NULL, 10, (uint16_t)(0));		// write to D1 with a 250Hz PWM signal

	redPin = &D[10];
	pin_digitalOut(redPin);
	oc_pwm(&oc2, redPin, NULL, 100, (uint16_t)(0));	
	greenPin = &D[8];
	pin_digitalOut(greenPin);
	oc_pwm(&oc3, greenPin, NULL, 100, (uint16_t)(0));	
	bluePin = &D[12];
	pin_digitalOut(bluePin);
	oc_pwm(&oc4, bluePin, NULL, 100, (uint16_t)(0));	


	// Motor controller pins
	dirPin = &D[6];
	pin_digitalOut(dirPin);
	nSleepPin = &D[5];
	pin_digitalOut(nSleepPin);
	pin_write(nSleepPin, 1);
	stepPin = &D[4];
	pin_digitalOut(stepPin);
	//oc_pwm(&oc5, stepPin, NULL, 100, (uint16_t)(0));

	testPin = &D[13];
	pin_digitalOut(testPin);


	pin_write(outPin, 10000);	//duty doesn't matter, really. 
	timer_setFreq(&timer1, 10000);

	while (1) {
		ServiceUSB(); // service any pending USB requests

		
		if (change == 1){
			timer_start(&timer1);
			change = 0;
		}
		

		// fade on when touched
		if (touching0 == 10){
			if (greenOn == 0){
				greenChange = 1;
			}
			if (greenOn == 1){
				greenChange = -1;
			}
			redChange = -1;
			blueChange = -1;
		}
		if (touching1 == 11){
			if (redOn == 1){
				redChange = -1;
			}
			if (redOn == 0){
				redChange = 1;
			}
			blueChange = -1;
			greenChange = -1;
		}
		if (touching2 == 12){
			if (blueOn == 1){
				blueChange = -1;
			}
			if (blueOn == 0){
				blueChange = 1;
			}
			greenChange = -1;
			redChange = -1;
		}
		greenDuty = greenDuty + greenChange;
		if (greenDuty == MAX_INT){
			greenDuty = MAX_INT -1;
			greenOn = 1;
			greenChange = 0;
		}
		if (greenDuty == 0){
			greenDuty = 1;
			greenOn = 0;
			greenChange = 0;
		}
		redDuty = redDuty + redChange;
		if (redDuty == MAX_INT){
			redDuty = MAX_INT - 1;
			redOn = 1;
			redChange = 0;
		}
		if (redDuty == 0){
			redDuty = 1;
			redOn = 0;
			redChange = 0;
		}
		blueDuty = blueDuty + blueChange;
		if (blueDuty == MAX_INT){
			blueDuty = MAX_INT - 1;
			blueOn = 1;
			blueChange = 0;
		}
		if (blueDuty == 0){
			blueDuty = 1;
			blueOn = 0;
			blueChange = 0;
		}
		pin_write(greenPin, greenDuty);
		pin_write(redPin, redDuty);
		pin_write(bluePin, blueDuty);

		/*
		// toggle between outputting to greenPin, redPin and bluePin, depending on the last touch.
		if (touching0 == 10){
			greenOn = 1;
			redOn = 0;
			blueOn = 0;
		}
		if (touching1 == 11){
			greenOn = 0;
			redOn = 1;
			blueOn = 0;
		}
		if (touching2 == 12){
			greenOn = 0;
			redOn = 0;
			blueOn = 1;
		}
		if (greenOn == 1){
			pin_write(greenPin, 64000);
		} else {
			pin_write(greenPin, 0);
		}
		if (redOn == 1){
			pin_write(redPin, 64000);
		} else {
			pin_write(redPin, 0);
		}
		if (blueOn == 1){
			pin_write(bluePin, 64000);
		} else {
			pin_write(bluePin, 0);
		}
		*/


		/*
		// toggle D4 output between all on, half on, and all off
		if (touching0 == 10){
			pin_write(greenPin, 64000);
		}
		if (touching1 == 11){
			pin_write(greenPin, 32000);
		}
		if (touching2 == 12){
			pin_write(greenPin, 1000);
		}
		*/
		

	}
}
