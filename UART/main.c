//=========================================================== //  Test program UART0 TX 8/N/1 @ 9600 bit/s
// LTL 17.5.2020
//=========================================================== // Include the Header File for controller tm4c1294ncpdt
// Vorname, Nachname => ??? Nathalie Mashal/Hilary Ogalagu/Emmanuel Duru
//==========================================================

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

void configPortA(void){
    // initialize Port A
    SYSCTL_RCGCGPIO_R |= (1<<0); // switch on clock for Port A
    while((SYSCTL_PRGPIO_R & (1<<0)) == 0); // wait for stable clock
    GPIO_PORTA_AHB_DEN_R |= 0x03; // digital I/O enable for pin PA(1:0)
    GPIO_PORTA_AHB_AFSEL_R |= 0x03; // PA(1:0) set to alternate func
    GPIO_PORTA_AHB_PCTL_R |= 0x00000011; // alternate func is U0Rx+Tx
}
void configUart0(void){
    // initialize UART0
    SYSCTL_RCGCUART_R |= (1<<0); // switch on clock for UART0
    while((SYSCTL_PRUART_R & (1<<0))==0); // wait for stable clock
    UART0_CTL_R &= ~(1<<0); // disable UART0 for config
    // initialize bitrate of 9600 bit per second
    UART0_IBRD_R = 104; // set DIVINT of BRD floor(16MHz/16/9600Hz)
    UART0_FBRD_R = 11; // set DIVFRAC of BRD
    UART0_LCRH_R = 0x00000040; // serial format 7N1
    UART0_CTL_R |= 0x0301; // re-enable UART0
}

void main(void){
    // config Port A
    configPortA();
    // config UART0 Rx+Tx
    configUart0();
    while(1){
        while((UART0_FR_R & (1<<5)) !=0); // till transmit FIFO empty
        UART0_DR_R = 'H'; // send 'H'
    }
}
