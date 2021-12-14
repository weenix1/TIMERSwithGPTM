//===========================================================
// Test program UART0 TX 8/N/1 @ 9600 bit/s
// LTL 17.5.2020
//==========================================================

#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

char buffer[16];
int i = 0;
int j = 0;

void activatePortJ(void){

    GPIO_PORTJ_AHB_IS_R &= ~0x01; // sense to "edge-sensitive"
    GPIO_PORTJ_AHB_IBE_R &= ~0x01; // trigger set to "single edge"
    GPIO_PORTJ_AHB_IEV_R |= 0x01; // interrupt event to rising edge
    GPIO_PORTJ_AHB_ICR_R |= 0x01; // clear interrupt PORTE(0)
    GPIO_PORTJ_AHB_IM_R |= 0x01; // unmask PORTE(0)
    NVIC_EN2_R |= (1<<8); // enable PortJ interrupt

}
void configPortA(void)
    {
     // initialize Port A
     SYSCTL_RCGCGPIO_R |= (1<<0); // switch on clock for Port A
     while((SYSCTL_PRGPIO_R & (1<<0)) == 0); // wait for stable clock
     GPIO_PORTA_AHB_DEN_R |= 0x03; // digital I/O enable for pin PA(1:0)
     GPIO_PORTA_AHB_AFSEL_R |= 0x03; // PA(1:0) set to alternate func
     GPIO_PORTA_AHB_PCTL_R |= 0x00000011; // alternate func is U0Rx+Tx
    }

void configUart0(void)
{
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

void configPorts(void) {
    SYSCTL_RCGCGPIO_R |= 0x1120 ;
    while((SYSCTL_RCGCGPIO_R & 0x1120) == 0);
    GPIO_PORTN_DEN_R = 0x03; // => enable port_F pin 0 and 1
    GPIO_PORTN_DIR_R = 0x03; // => output pin 0 and 1
    GPIO_PORTF_AHB_DEN_R = 0x11; // => enable Port_F pin 0 and 4
    GPIO_PORTF_AHB_DIR_R = 0x11; // => output at pin 0 and 4

    GPIO_PORTJ_AHB_DEN_R = 0x03; // => enable port_J pin 0 and 1
    GPIO_PORTJ_AHB_PUR_R = 0x03; // WPU for PIN PJ0 and PJ1
    GPIO_PORTJ_AHB_DIR_R &= ~(0x03); // => input at pin 0 and 1
}
void cmdToLED(char buffer[], int size){ // to check and Convert CMD to LED Activation
    int k=0;
    for (k=size;k <= 16; k++){

        if ((buffer[k] == '+') && (buffer[k+1] == '1')) GPIO_PORTN_DATA_R |= 0x02;
        if ((buffer[k] == '+') && (buffer[k+1] == '2')) GPIO_PORTN_DATA_R |= 0x01;
        if ((buffer[k] == '-') && (buffer[k+1] == '1')) GPIO_PORTN_DATA_R &= ~(0x02);
        if ((buffer[k] == '-') && (buffer[k+1] == '2')) GPIO_PORTN_DATA_R &= ~(0x01);
        if ((buffer[k] == '+') && (buffer[k+1] == '3')) GPIO_PORTF_AHB_DATA_R |= 0x010;
        if ((buffer[k] == '+') && (buffer[k+1] == '4')) GPIO_PORTF_AHB_DATA_R |= 0x01;
        if ((buffer[k] == '-') && (buffer[k+1] == '3')) GPIO_PORTF_AHB_DATA_R &= ~(0x010);
        if ((buffer[k] == '-') && (buffer[k+1] == '4')) GPIO_PORTF_AHB_DATA_R &= ~(0x01);

        if ((buffer[k] == '~') && (buffer[k+1] == '1')) GPIO_PORTN_DATA_R ^= 0x02;
        if ((buffer[k] == '~') && (buffer[k+1] == '2')) GPIO_PORTN_DATA_R ^= 0x01;
        if ((buffer[k] == '~') && (buffer[k+1] == '3')) GPIO_PORTF_AHB_DATA_R ^= 0x010;
        if ((buffer[k] == '~') && (buffer[k+1] == '4')) GPIO_PORTF_AHB_DATA_R ^= 0x01;

    }

}
void main(void)
{
     // config Port A
     configPortA();
     // config UART0 Rx+Tx
     configUart0();
     configPorts();
     activatePortJ();
     unsigned char drain_letter ;
     int enter = 0;

     while(1){
         while((UART0_FR_R & (1<<5)) !=0); // till transmit FIFO empty
         UART0_DR_R = '>'; // send 'H'

         while(i <= 15){

             while((UART0_FR_R & (1<<4)) && (portJHandler() != 1));// wait till Rx Fifo is empty and Itereupt not occured
             buffer[i]=UART0_DR_R; // Put Uart Rx data in buffer
             enter = buffer[i]; // enter key variable

             if (i >= 15){
             buffer[i]=0x0; // if buffer full
             drain_letter = UART0_DR_R;// last data to be drained through this variable
             break;
             }

             if (enter == 13) {// Enter key pressed
             buffer[i] = 0x00; // console input for EOT = Strg+D
             break; // replace last element by \0 to terminate string
             }
             i++;
         }
         printf(" %s\n",buffer); // show rx data on console
         i=0; // reset iterator for buffer size
         int k = 0;
         for(k=0; k<=16; k++){
             if ((buffer[k] == 'l') && (buffer[k+1] == 'e') && (buffer[k+2] == 'd') && (buffer[k+3] == 's')){ // check if leds cmd passed
                 cmdToLED(buffer, k); // call Cmd to Led function
             }
         }

         for (k=0; k<=16; k++) {
             buffer[k] = '0'; // reset buffer
         }
     }
}

