//=========================================================== // Example Program for the TIVA TM4C1294XL Evaluation Board
// With this file the LEDs D0 to D3 on the eval board will
// blink each one after another. To control the LEDs the
// Ports N and F with Pins PN0, PN1, PF0 and PF4 have to be
// enabled and set as digital output.
//=========================================================== // Include the Header File for controller tm4c1294ncpdt
// Vorname, Nachname => ??? Nathalie Mashal/Hilary Ogalagu/Emmanuel Duru

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
//Prototypes
void config_gpio(void);
void config_timer(void);

int main(void) {

    //Initialization
    config_gpio();
    config_timer();
    unsigned char pattern[]= {0x00,0x01,0x10,0x11};
    unsigned char pattern2[]= {0x00,0x01,0x02,0x03};
    int i= 0;
    int j=0;

    //LED control
    unsigned int counter = 0;
    while(1) {
        counter++;

        if (counter <= 3){

            GPIO_PORTF_AHB_DATA_R=pattern[counter]; // turn on LEDs (selected pins high) according to the bit pattern of the counter
            while ((TIMER1_RIS_R &(1<<0)) ==0);
            TIMER1_ICR_R |= (1<<0);
            GPIO_PORTF_AHB_DATA_R=pattern[0];

        }
        else {
            for(i=0; i<=2;i++){
                for (j=0;j<=3;j++){

                    GPIO_PORTN_DATA_R=pattern2[i];
                    GPIO_PORTF_AHB_DATA_R=pattern[j];
                    while ((TIMER1_RIS_R &(1<<0)) ==0);
                    TIMER1_ICR_R |= (1<<0);
                }
            }
            // wait for the timeout event
            GPIO_PORTN_DATA_R=0x0;
            GPIO_PORTF_AHB_DATA_R=pattern[0];
            counter=0;
        }


    }

}
void config_gpio(){

    SYSCTL_RCGCGPIO_R |= 0x1020 ;
    while((SYSCTL_RCGCGPIO_R & 0x1020) == 0);
    // Kick out the switches
    GPIO_PORTN_DEN_R = 0x03; //enable portN pin 0 and 1
    GPIO_PORTN_DIR_R = 0x03; //output pin 0 and 1
    GPIO_PORTF_AHB_DEN_R = 0x11; //enable portF pin 0 and 4
    GPIO_PORTF_AHB_DIR_R = 0x11; //output pin 0 and 4
}

void config_timer(){


    SYSCTL_RCGCTIMER_R |= (1<<1);
    while ((SYSCTL_PRTIMER_R & (1<<1))==0);

    TIMER1_CTL_R &= ~(0x0001); //disable timer
    TIMER1_CFG_R = 0x04; // 2x16 bit
    TIMER1_TAMR_R |= 0x12;
    TIMER1_TAPR_R = 245-1; // prescaler PR= ceil(16M/2^16*1)-1 = ceil(244,14)-1
    TIMER1_TAILR_R = 65306-1;//ILR= round(16M/245*1)-1
    TIMER1_CTL_R |= 0x0001; // enable timer

}
