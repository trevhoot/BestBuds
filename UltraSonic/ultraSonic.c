#include <p24FJ128GB206.h>
#include "..\lib\config.h"
#include "..\lib\common.h"
#include "..\lib\pin.h"
#include "..\lib\oc.h"
#include "..\lib\ui.h"
#include "..\lib\timer.h"
#include "..\lib\usb.h"
#include "..\lib\uart.h"
#include <stdio.h>

#define SEND        0   // Vendor request that receives 2 unsigned integer values
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns 2 unsigned integer values
#define SCAN        3   // Vendor request that returns 2 unsigned integer values

uint16_t dc = 32767, val1, val2, val3, val4;

void send(_TIMER *self) {
    pin_write(&D[4],0);
    timer_stop(&timer4);
}

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SEND:
            timer_stop(&timer5);
            pin_write(&D[4], dc);
            timer_after(&timer4, 500e-6, 1, send);
            timer_setPeriod(&timer5, 50e-3);
            timer_start(&timer5);
            led_toggle(&led1);
            printf("sent a pulse\n");
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_VALS:
            val1 = USB_setup.wValue.w;
            val2 = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = val1;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = val2;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SCAN:
            pin_write(&D[4], dc);
            timer_after(&timer4, 500e-6, 1, send);
            timer_setPeriod(&timer5, 50e-3);
            timer_start(&timer5);
            led_toggle(&led1);
            while(pin_read(&D[12]) == 0){
                if (timer_flag(&timer5) != 0){
                    led_toggle(&led3);
                    break;
                }
            }
            val3 = timer_read(&timer5);
            led_toggle(&led2);
            timer_stop(&timer5);
            temp.w = val3;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = val4;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;

        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();
    init_oc();
    init_pin();
    init_uart();

    val1 = 0;
    val2 = 0;
    val3 = 0;
    val4 = 0;
    pin_digitalOut(&D[0]);
    pin_digitalOut(&D[1]);
    pin_digitalOut(&D[4]);
    oc_servo(&oc1, &D[0], &timer1, 0.01, 800e-6, 2200e-6, 0);
    oc_servo(&oc2, &D[1], &timer2, 0.01, 800e-6, 2200e-6, 0);
    oc_pwm(&oc3, &D[4], &timer3, 40000, 0);

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

    while(1){
        ServiceUSB();
        pin_write(&D[4],32767);
        pin_write(&D[0], val1);
        pin_write(&D[1], val2);
    }
}