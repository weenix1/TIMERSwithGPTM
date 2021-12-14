#include "inc/tm4c1294ncpdt.h"
#include <stdint.h>
#include <stdio.h>

char led_status [5] = {'#'};
int portJHandler(void){ //

    int counter = 0;
    int loop_counter = 0;
    if ((GPIO_PORTJ_AHB_DATA_R & 0x01)==0){ // check if port J data register changed

        int portf_value = GPIO_PORTF_AHB_DATA_R; // put led data in variable as decimal
        int portn_value = GPIO_PORTN_DATA_R;

        if (portn_value == 3) { // for LED 1 and 2 both
            counter ++;
            led_status[counter] = '1';
            counter ++;
            led_status[counter]='2';
        }
        // in case one LED is on
        else if (portn_value == 2){
            counter ++;
            led_status[counter] = '1';
        }
        else if (portn_value == 1){
            counter ++;
            led_status[counter]='2';
        }

        if (portf_value == 17){ // for LED 3 and 4
            counter ++;
            led_status[counter] = '3';
            counter ++;
            led_status[counter]='4';
        }
        // in case one LED is on
        else if (portf_value == 16){
            counter ++;
            led_status[counter] = '3';
        }
        else if (portf_value == 1){
            counter ++;
            led_status[counter] = '4';
        }

        led_status[counter+1]='\0'; // EOT

        for (loop_counter;loop_counter <= counter; loop_counter++){
            while((UART0_FR_R & (1<<5)) !=0); // till transmit FIFO empty
             UART0_DR_R = led_status[loop_counter]; // send led status
             GPIO_PORTJ_AHB_ICR_R |= 0x01;
        }


        return 1;
       }

    return 0;
}


