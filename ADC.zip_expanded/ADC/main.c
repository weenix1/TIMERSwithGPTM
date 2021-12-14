//=========================================================== // Example Program for the TIVA TM4C1294XL Evaluation Board
// With this file the LEDs D0 to D3 on the eval board will
// blink each one after another. To control the LEDs the
// Ports N and F with Pins PN0, PN1, PF0 and PF4 have to be
// enabled and set as digital output.
//=========================================================== // Include the Header File for controller tm4c1294ncpdt
// Vorname, Nachname => ??? Nathalie Mashal/Hilary Ogalagu/Emmanuel Duru

#include <stdint.h>
#include "inc/tm4c1294ncpdt.h"
#include <stdio.h>

void configADC(void);
void config_timer(void);

void main(void) {

    unsigned int ulData;
    unsigned int avData;
    configADC();
    config_timer();
    int i=0;

    while(1) {
        while((TIMER1_RIS_R & (1<<0))==0);//make this non blocking
        TIMER1_ICR_R |= (1<<0);
        ADC0_PSSI_R |= 0x01; // Start ADC0

        //while(ADC0_SSFSTAT0_R &(1<<8));// wait for FIFO "NON EMPTY"
        //
        ulData=0;

        for ( i=0;i<=4;i++){
           while (ADC0_SSFSTAT0_R & (1<<8));
            ulData+=ADC0_SSFIFO0_R;

        }
        avData = 147.5-((75*3.3)*(ulData/4))/4096;
        printf(" TEMP= %d C\n",avData);


    }
}
void configADC(){
    int waitcycle=0;

    SYSCTL_RCGCGPIO_R |= (1<<4); // PE (AIN0 to AIN2 belong to Port E)
    while(!(SYSCTL_PRGPIO_R & 0x10)); // Ready ?
    SYSCTL_RCGCADC_R |= (1<<0); // ADC0 digital block
    while(!(SYSCTL_PRADC_R & 0x01)); // Ready ?

    // configure AIN0 to AIN2 (= PE(3..1)) as analog inputs
    //GPIO_PORTE_AHB_AFSEL_R |=0x0E; // PE3..3 Alternative Pin Function enable
    //GPIO_PORTE_AHB_DEN_R &= ~0x0E; // PE3..1 disable digital IO
    //GPIO_PORTE_AHB_AMSEL_R |= 0x0E; // PE3..1 enable analog function
    //GPIO_PORTE_AHB_DIR_R &= ~0x0E; // Allow Input PE3 ..1

    // ADC0_SS0 configuration
    ADC0_ACTSS_R &= ~0x0F; // disable all 4 sequencers of ADC0

    SYSCTL_PLLFREQ0_R |= (1<<23); // PLL Power
    while (!(SYSCTL_PLLSTAT_R & 0x01)); // until PLL has locked
    ADC0_CC_R |= 0x01;
    waitcycle++; // PIOSC for ADC sampling clock
    SYSCTL_PLLFREQ0_R &= ~(1<<23); // PLLPoweroff
    ADC0_SSMUX0_R |=    0x00000102; // sequencer 0, channel AIN2,AIN0,AIN1
    ADC0_SSCTL0_R |=0x00000100; // END2 set, sequence length = 3
    ADC0_ACTSS_R |=0x01; // enable sequencer 0 ADC0

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

