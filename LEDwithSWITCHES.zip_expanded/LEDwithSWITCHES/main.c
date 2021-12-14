//=========================================================== // Example Program for the TIVA TM4C1294XL Evaluation Board
// With this file the LEDs D0 to D3 on the eval board will
// blink each one after another. To control the LEDs the
// Ports N and F with Pins PN0, PN1, PF0 and PF4 have to be
// enabled and set as digital output.
//=========================================================== // Include the Header File for controller tm4c1294ncpdt
// Vorname, Nachname => ??? ......... ............

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
void main(void)
{

    int i; // => initialization of the variable

    SYSCTL_RCGCGPIO_R |= 0x00001120 ; // =>

    while((SYSCTL_RCGCGPIO_R & 0x00001120) == 0);
    GPIO_PORTN_DEN_R = 0x03; // enable portN PIN PJ0 and PJ1
    GPIO_PORTN_DIR_R = 0x03; // output  PIN PJ0 and PJ1
    GPIO_PORTF_AHB_DEN_R = 0x11; // enable portF PIN PJ0 and PJ4

    GPIO_PORTF_AHB_DIR_R = 0x11; // output PIN PJ0 and PJ4
    GPIO_PORTJ_AHB_DIR_R &= ~0x03; // input  PIN PJ0 and PJ1
    GPIO_PORTJ_AHB_DEN_R = 0x03; // enable portJ PIN PJ0 and PJ1
    GPIO_PORTJ_AHB_PUR_R = 0x03; //  WPU for PIN PJ0 and PJ1

    //GPIO_PORTJ_AHB_PUR_R = 0x03; // WPU for PIN PJ0 and PJ1

    while(1)
    {
        if ((GPIO_PORTJ_AHB_DATA_R & 0x01)== 0
                && (GPIO_PORTJ_AHB_DATA_R & 0x02)!= 0) // J0
        {
            GPIO_PORTF_AHB_DATA_R = 0x00;
            GPIO_PORTN_DATA_R = 0x02; // => ???
            for(i=0;i<500000;i++); // => ???

            GPIO_PORTN_DATA_R = 0x01; // => ???
            for(i=0;i<500000;i++); // => ???
            GPIO_PORTN_DATA_R = 0x00; // => reset portN

        }

        if ((GPIO_PORTJ_AHB_DATA_R & 0x02)== 0
                && (GPIO_PORTJ_AHB_DATA_R & 0x01)!= 0) // J1
        {
            GPIO_PORTN_DATA_R = 0x00;
            GPIO_PORTF_AHB_DATA_R = 0x10; // => ???
            for(i=0;i<500000;i++); // => ???

            GPIO_PORTF_AHB_DATA_R = 0x01; // => ???
            for(i=0;i<500000;i++); // => ???

            GPIO_PORTF_AHB_DATA_R = 0x00; // => reset portF
        }

        if (GPIO_PORTJ_AHB_DATA_R == 0x00)

        {
            GPIO_PORTN_DATA_R = 0x02; // portN
            GPIO_PORTF_AHB_DATA_R = 0x10; //portF

            for(i=0;i<500000;i++); // => ???
            GPIO_PORTN_DATA_R = 0x01;
            GPIO_PORTF_AHB_DATA_R = 0x01;
            for(i=0;i<500000;i++); // => ???
            GPIO_PORTN_DATA_R = 0x00; // reset portN
            GPIO_PORTF_AHB_DATA_R = 0x00; // reset portF
        }

    }
}
