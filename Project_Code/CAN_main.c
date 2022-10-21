/*
 * File:   CAN_main.c
 * Author: ma70436
 *
 * Created on October 11, 2022, 3:04 PM
 */



// PIC18F4580 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = HS         // Oscillator Selection bits (HS oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = BOHW     // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config BBSIZ = 1024     // Boot Block Size Select bit (1K words (2K bytes) boot block)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 20000000

int CAN_set_baud(int baud)
{
    CANCON = 0xE0;                    // CAN Configuration mode and  Receive Buffer 0
    while(!((CANSTAT &0x80) == 0x80)); // wait until the CAN set to Configuration mode
    
    if(baud == 125){
    /* 125KHz*/
        BRGCON1 = 0xC1;
        BRGCON2 = 0xB1;
        BRGCON3 = 0x45;
        if( (BRGCON1 == 0xC1) && (BRGCON2 == 0xB1) && (BRGCON3 == 0x45))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    
    if(baud == 250){
    /* 250KHz*/
        BRGCON1 = 0xC1;
        BRGCON2 = 0xAE;
        BRGCON3 = 0xA5;
    
        if( (BRGCON1 == 0xC1) && (BRGCON2 == 0xAE) && (BRGCON3 == 0xA5))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    
    CIOCON = 0x20;
    CANCON = 0x08;  // getting into CAN Normal mode
    
    return 0;
}

/*
 * CAN Initialization
 */
void CAN_Init()
{
    TRISBbits.RB2 = 0;  // CAN TX
    TRISBbits.RB3 = 1;  // CAN RX
}

/*
 * CAN_Send_msg is used to transmit the data on the bus
 * attributes "type" define mode of bus
 *  1 - Standard mode
 *  0 - Extended mode 
 * "ID" is nothing but Identifier
 */
void CAN_Send_msg(char type,int ID,int *data)
{
//   int data[8];
//   int index=0;
//   for(index=0;index<8;index++)
//   {
//       data[index] = 0x60 + index;
//   }
   
    CANCON = 0x08;  // getting into CAN Normal mode     
    if(type)    /* Standard Mode */
    {
        TXB0SIDH = ((ID >> 3) & 0xFF);
        TXB0SIDL = ((ID << 5) & 0xFF);
        
        __delay_ms(10); /* making some 10 ms delay to set Identifier*/
        
        TXB0DLC = 0x08; /* Data Length 8 */
        
        /* Filling with data in transmit buffers */
        TXB0D0 = data[0];
        TXB0D1 = data[1];
        TXB0D2 = data[2];
        TXB0D3 = data[3];
        TXB0D4 = data[4];
        TXB0D5 = data[5];
        TXB0D6 = data[6];
        TXB0D7 = data[7];
        
        TXB0CON = 0x08;  /* Request to send the data */
        
         __delay_ms(10); /* making some 10 ms delay*/
        
        CANCON = 0x08;  // getting into CAN Normal mode     
    }
    else  /* Extended Mode */
    {
        
    }
}

void main(void) {
    
   int data[8],buff[8]; /* declaring buffers */
   int index=0;
   for(index=0;index<8;index++)
   {
       data[index] = 0x60 + index;  /* defining buffers data */
       buff[index] = 0x31 + index;
   }
   
   RBPU =1;
   TRISBbits.RB0 = 1;   /* RD2 is connected with switch and taking as input */
   

   CAN_Init();        /* Initializing CAN */ 
   CAN_set_baud(250); /* Setting Baud rate as 250 kbps */
   
   while(1)
   {
       if(PORTBbits.RB0)
       {
          CAN_Send_msg(1,0x100,buff);
       }
       else{
          CAN_Send_msg(1,0x100,data);
       }
       __delay_ms(1000);    /* setting delay of 1Sec  */
   }
   
    return;
}
