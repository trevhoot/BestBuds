#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "usb.h"
#include "oc.h"
#include "ui.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define SET_TIMER 0
#define GET_STATE 1
#define SET_THRESHOLD 2
#define GET_VOLTAGE 3
#define SET_DUTY 4
#define GET_STEP_COUNT 5
#define SCAN 6
#define SET_SPEED 7
#define DEBUG 8

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
_PIN * inPin3 = 0;
_PIN * inPin4 = 0;
_PIN * testPin = 0;
_PIN * stepPin = 0;
_PIN * dirPin = 0;
_PIN * nSleepPin = 0;
_PIN * pingPin = 0;
_PIN * receivePin = 0;

// Global variables;
float period = 0;
int dir = 0;
uint16_t destination;
int step = 0;
uint16_t stepCount = HALF_INT;
int change = 0;
int motorOn = 0;
int testVar = 0;
int flag = 0;
int count = 0;

uint16_t Vin0 = 0, Vin1 = 0, Vin2 = 0, Vin3 = 0, Vin4 = 0;
uint16_t VatTime0 = 0, VatTime1 = 0, VatTime2 = 0, VatTime3 = 0, VatTime4 = 0;
int touching0 = 0, touching1 = 0, touching2 = 0, touching3 = 0, touching4 = 0;
uint16_t threshold = 0;
int petal = 0;

int greenOn = 0, redOn = 0, blueOn = 0;
int greenChange = 0, redChange = 0, blueChange = 0;
uint16_t greenDuty = 0, redDuty = 0, blueDuty = 0;
uint16_t greenTarget = 0, redTarget = 0, blueTarget = 0;
uint16_t currentPetal = 5;

uint16_t pingTime = 0, waitTime = 0, readTime = 0;
uint16_t open = 32668, closed = 32468;


void receive(_TIMER *self) {
	timer_setPeriod(&timer5, 50e-3);
	timer_start(&timer5);
	led_toggle(&led1);
	while(pin_read(&D[12]) == 0){
		if (timer_flag(&timer5) != 0){
			led_toggle(&led3);
			break;
		}
	}
	readTime = timer_read(&timer5);
	led_toggle(&led2);
	timer_stop(&timer5);
}

void send(_TIMER *self) {
	pin_write(pingPin,0);
	timer_stop(&timer4);
	timer_after(&timer2, 1.5e-3, 1, receive);
}
/*
void ping() {
	pin_write(pingPin, HALF_INT);
	timer_after(&timer4, 500e-6, 1, send);	
}
*/
void motorControl(destination) {	

	while (stepCount != destination){
		
		
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
		timer_start(&timer1);
		led_toggle(&led1);
		
		while (1) {
			count = timer_read(&timer1);
			led_toggle(&led2);
			if (count > period){
				break;
			}
		}
		timer_stop(&timer1);
		timer_lower(&timer1);
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
		petal = 3;
	}
	else if (petal == 3){
		Vin3 = pin_read(inPin3);
		if (Vin3 != 0){
			VatTime3 = Vin3;
			if (Vin3 > threshold){
				touching3 = 3;
			} else {
				touching3 = 13;
			}
		}
		petal = 4;
	}
	else if (petal == 4){
		Vin4 = pin_read(inPin4);
		if (Vin4 != 0){
			VatTime4 = Vin4;
			if (Vin4 > threshold){
				touching4 = 4;
			} else {
				touching4 = 14;
			}
		}
		petal = 0;
	}
}

void VendorRequests(void) {
	WORD temp;

	switch (USB_setup.bRequest) {
		case GET_VOLTAGE:
			temp.w = VatTime0;
			BD[EP0IN].address[0] = VatTime0/256;
			BD[EP0IN].address[1] = VatTime1/256;
			BD[EP0IN].address[2] = VatTime2/256;
			BD[EP0IN].address[3] = VatTime3/256;
			BD[EP0IN].address[4] = VatTime4/256;
			BD[EP0IN].bytecount = 5;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break; 
		case GET_STATE:
			BD[EP0IN].address[0] = touching0;
			BD[EP0IN].address[1] = touching1;
			BD[EP0IN].address[2] = touching2;
			BD[EP0IN].address[3] = touching3;
			BD[EP0IN].address[4] = touching4;
			BD[EP0IN].bytecount = 5;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case DEBUG:
			temp.w = readTime;
			BD[EP0IN].address[0] = temp.b[0];
			BD[EP0IN].address[1] = temp.b[1];
			temp.w = destination;
			BD[EP0IN].address[2] = temp.b[0];
			BD[EP0IN].address[3] = temp.b[1];
			BD[EP0IN].address[4] = 0;
			BD[EP0IN].address[5] = greenDuty;
			BD[EP0IN].address[6] = redDuty;
			BD[EP0IN].address[7] = blueDuty;
			BD[EP0IN].bytecount = 8;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case SET_THRESHOLD:	
			threshold = USB_setup.wValue.w;
			BD[EP0IN].bytecount = 0;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case SET_DUTY:	
			destination = USB_setup.wValue.w;
			motorControl(destination);
			BD[EP0IN].bytecount = 0;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case GET_STEP_COUNT:
			temp.w = stepCount;
		   	BD[EP0IN].address[0] = temp.b[0];
			BD[EP0IN].address[1] = temp.b[1];
			temp.w = destination;
		   	BD[EP0IN].address[2] = temp.b[0];
			BD[EP0IN].address[3] = temp.b[1];
			BD[EP0IN].bytecount = 4;	// set EP0 IN byte count to 0
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;
		case SCAN:
			pin_write(pingPin, HALF_INT);
			timer_after(&timer4, 500e-6, 1, send);
			/*timer_setPeriod(&timer5, 50e-3);
			timer_start(&timer5);
			led_toggle(&led1);
			while(pin_read(&D[12]) == 0){
				if (timer_flag(&timer5) != 0){
					led_toggle(&led3);
					break;
				}
			}
			readTime = timer_read(&timer5);
			led_toggle(&led2);
			timer_stop(&timer5);
			temp.w = readTime;
			BD[EP0IN].address[0] = temp.b[0];
			BD[EP0IN].address[1] = temp.b[1];
			*/
			BD[EP0IN].bytecount = 2;	// set EP0 IN byte count to 4
			BD[EP0IN].status = 0xC8;	// send packet as DATA1, set UOWN bit
			break;	
		case SET_SPEED:	
			period = USB_setup.wValue.w;
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
	init_ui();

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
	timer_enableInterrupt(&timer2);
	timer_lower(&timer2);
	timer_enableInterrupt(&timer4);
	timer_lower(&timer4);
	timer_enableInterrupt(&timer5);
	timer_lower(&timer5);



	// Configure Pins
	inPin0 = &A[0];	
	pin_analogIn(inPin0);
	inPin1 = &A[1];	
	pin_analogIn(inPin1);
	inPin2 = &A[2];	
	pin_analogIn(inPin2);
	inPin3 = &A[3];	
	pin_analogIn(inPin3);
	inPin4 = &A[4];	
	pin_analogIn(inPin4);

	outPin = &D[6];
	pin_digitalOut(outPin);
	oc_pwm(&oc1, outPin, NULL, 10, (uint16_t)(0));		// write to D1 with a 250Hz PWM signal
	pin_write(outPin, 10000);	//duty doesn't matter, really. 

	redPin = &D[7];
	pin_digitalOut(redPin);
	oc_pwm(&oc2, redPin, NULL, 100, (uint16_t)(0));	
	greenPin = &D[10];
	pin_digitalOut(greenPin);
	oc_pwm(&oc3, greenPin, NULL, 100, (uint16_t)(0));	
	bluePin = &D[8];
	pin_digitalOut(bluePin);
	oc_pwm(&oc4, bluePin, NULL, 100, (uint16_t)(0));	

	pingPin = &D[4];
	pin_digitalOut(pingPin);
	oc_pwm(&oc5, pingPin, &timer3, 40000, 0);
	receivePin = &D[12];
	pin_digitalIn(receivePin);

	// Motor controller pins
	dirPin = &D[0];
	pin_digitalOut(dirPin);
	nSleepPin = &D[3];
	pin_digitalOut(nSleepPin);
	pin_write(nSleepPin, 1);
	stepPin = &D[2];
	pin_digitalOut(stepPin);

	testPin = &D[13];
	pin_digitalOut(testPin);

	timer_setFreq(&timer1, 100);


	while (1) {
		ServiceUSB(); // service any pending USB requests

		
		//destination = readTime / (1000 - 400) * (open - closed);
		if (readTime < 400) {
			destination = open;
		} else if (readTime > 400) {
			destination = closed;
		}
		if (destination != stepCount) {
			motorControl(destination);
		}
		/*int change = abs(destination - stepCount);
		if ( change > 30) {
			motorControl(destination);
		}
		*/

		if (touching0 == 10){
			greenTarget = 40000;
			redTarget = 60000;
			blueTarget = 0;
			if (currentPetal == 0){
				greenTarget = 0;
				redTarget = 0;
				blueTarget = 0;
			}
		}
		if (touching1 == 11){
			greenTarget = 20000;
			redTarget = 20000;
			blueTarget = 20000;
			if (currentPetal == 1){
				greenTarget = 0;
				redTarget = 0;
				blueTarget = 0;
			}
		}
		if (touching2 == 12){
			greenTarget = 60000;
			redTarget = 0;
			blueTarget = 10000;
			if (currentPetal == 2){
				greenTarget = 0;
				redTarget = 0;
				blueTarget = 0;
			}
		}
		if (touching3 == 13){
			greenTarget = 0;
			redTarget = 60000;
			blueTarget = 0;
			if (currentPetal == 3){
				greenTarget = 0;
				redTarget = 0;
				blueTarget = 0;
			}
		}
		if (touching4 == 14){
			greenTarget = 0;
			redTarget = 60000;
			blueTarget = 40000;
			if (currentPetal == 4){
				greenTarget = 0;
				redTarget = 0;
				blueTarget = 0;
			}
		}
		if (greenDuty < greenTarget) {
			greenChange = 1;
		} else if (greenDuty > greenTarget) {
			greenChange = -1;
		} else {greenChange = 0;}
		if (redDuty < redTarget) {
			redChange = 1;
		} else if (redDuty > redTarget) {
			redChange = -1;
		} else {redChange = 0;}
		if (blueDuty < blueTarget) {
			blueChange = 1;
		} else if (blueDuty > blueTarget) {
			blueChange = -1;
		} else {blueChange = 0;}
		greenDuty += greenChange;
		redDuty += redChange;
		blueDuty += blueChange;
		pin_write(greenPin, greenDuty);
		pin_write(redPin, redDuty);
		pin_write(bluePin, blueDuty);


		/*
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
		*/

		/*
		if (iteration > 10000) {
			ping();
			iteration = 0;
		}
		iteration += 1;
		*/
	}
}
