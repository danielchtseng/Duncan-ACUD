
// ***********************************************************************
// ACUD.C
//
// Keil C51 
// Device: AT89C51RC
// Target: 
//		Xtal: 22M
//		Memory Model: Large, 
//		Code Rom Size: Large
//		Use On-Chip ROM: Enable
//		Use On-Chip RAM: Enable
//
// Author	: Duncan Tseng
// DateTime	: W143, H11450
//
// ***********************************************************************


// @@@@@@@@@@@@@@@      include      @@@@@@@@@@@@@@@

#include <AT89X51.h>
#include <string.h>
#include <math.h>
//#include <stdbool.h>
//#include <stdlib.h>


// @@@@@@@@@@@@@@@      Declare      @@@@@@@@@@@@@@@

/* Keil C Data types
Data Types				Bits	Bytes	Value Range
bit						1	 			0 to 1
signed char				8		1		-128 — +127
unsigned char			8		1		0 — 255
enum					8/16	1/2		-128 — +127 or -32768 — +32767
signed short [int]		16		2		-32768 — +32767
unsigned short [int]	16		2		0 — 65535
signed int				16		2		-32768 — +32767
unsigned int			16		2		0 — 65535
signed long [int]		32		4		-2147483648 — +2147483647
unsigned long [int]		32		4		0 — 4294967295
float					32		4		±1.175494E-38 — ±3.402823E+38
double					32		4		±1.175494E-38 — ±3.402823E+38
sbit					1	 			0 or 1
sfr						8		1		0 — 255
sfr16					16		2		0 — 65535
*/

/* Special Function Register that Declared in reg51.h
sfr 	P0   	= 0x80;
sfr 	P1   	= 0x90;
sfr 	P2   	= 0xA0;
sfr 	P3   	= 0xB0;
sfr 	PSW  	= 0xD0;
sfr 	ACC  	= 0xE0;
sfr 	B    	= 0xF0;
sfr 	SP   	= 0x81;
sfr 	DPL  	= 0x82;
sfr 	DPH  	= 0x83;
sfr 	PCON 	= 0x87;
sfr 	TCON 	= 0x88;
sfr 	TMOD 	= 0x89;
sfr 	TL0  	= 0x8A;
sfr 	TL1  	= 0x8B;
sfr 	TH0  	= 0x8C;
sfr 	TH1  	= 0x8D;
sfr 	IE   	= 0xA8;
sfr 	IP   	= 0xB8;
sfr 	SCON 	= 0x98;
sfr 	SBUF 	= 0x99;
*/

/* SPF BIT that Declared in reg51.h
// PSW   
sbit 	CY   	= 0xD7;
sbit 	AC   	= 0xD6;
sbit 	F0   	= 0xD5;
sbit 	RS1  	= 0xD4;
sbit 	RS0  	= 0xD3;
sbit 	OV   	= 0xD2;
sbit 	P    	= 0xD0;

// TCON 
sbit 	TF1  	= 0x8F;
sbit 	TR1  	= 0x8E;
sbit 	TF0  	= 0x8D;
sbit 	TR0  	= 0x8C;
sbit 	IE1  	= 0x8B;
sbit 	IT1  	= 0x8A;
sbit 	IE0  	= 0x89;
sbit 	IT0  	= 0x88;

// SCON 
sbit 	SM0  	= 0x9F;
sbit 	SM1  	= 0x9E;
sbit 	SM2  	= 0x9D;
sbit 	REN  	= 0x9C;
sbit 	TB8  	= 0x9B;
sbit 	RB8  	= 0x9A;
sbit 	TI   	= 0x99;
sbit 	RI   	= 0x98;

// IP  
sbit 	PS   	= 0xBC;							// sbit PS  = 0xB8^4
sbit 	PT1  	= 0xBB;							// sbit PT1 = 0xB8^3
sbit 	PX1  	= 0xBA;							// sbit PX1 = 0xB8^2
sbit 	PT0  	= 0xB9;							// sbit PT0 = 0xB8^1
sbit 	PX0  	= 0xB8;							// sbit PX0 = 0xB8^0

// IE  
sbit 	EA   	= 0xAF;							// sbit EA  = 0xA8^7
sbit 	ES   	= 0xAC;							// sbit ES  = 0xA8^4
sbit 	ET1  	= 0xAB;							// sbit ET1 = 0xA8^3
sbit 	EX1  	= 0xAA;							// sbit EX1 = 0xA8^2
sbit 	ET0  	= 0xA9;							// sbit ET0 = 0xA8^1
sbit 	EX0  	= 0xA8;							// sbit EX0 = 0xA8^0
*/

	
#ifndef MYBOOLEAN_H
	#define MYBOOLEAN_H
	#define false 0
	#define true 1
	typedef int bool; // or #define bool int
#endif	


/* Declare related to Timer */
unsigned short 	data	Ten_mS_Counter;					// 2 bytes: 0-65535


/* ########## Declare related to Communication */

/* Bit Field: 是一種省空間的特殊 data member, 可以使用特定幾個 bit 來放 data.
// The declaration of a bit-field has the following form inside a structure
// 		struct {
//   		type [data member] : width ;
// 		};
//
// For example
//      struct {
//			unsigned int age : 1;
//		} Flag Name;   						
*/

//*union {										// union: all variables in union share same memory
	/* Note: data type must be signed or unsigned */
//*	unsigned char	Comm_Flag;
	struct {		
		// relate to PC
		unsigned char PC_Tx_Busy_Flg 	: 1;				
		unsigned char PC_Rx_Ready_Flg 	: 1;
		/* relate to ACP */
		unsigned char ACP_Tx_Busy_Flg 	: 1;		
		unsigned char ACP_Rx_Ready_Flg 	: 1;	
	}Comm; 	
//*} Comm;


/* ########## Declare related to UART */
#define Fosc				22.1184				// ***** Need to be confirmed (0r 11.0592 )
#define Baudrate			9600				// ***** Need to be confirmed
#define PC_In_Buf_Max 		5					// ***** Need to be confirmed
#define PC_Out_Buf_Max		5					// ***** Need to be confirmed
#define Enter				13					// ASCII 13: carry Return
sbit 			PC_485Tx   		= P3^2;					// sbit INT0    = 0xB2;			// UART TXD
sbit 			UART_TxD1 		= P3^1;					// sbit TXD     = 0xB1;			// UARD RXD
sbit 			UART_RxD1 		= P3^0;					// sbit RXD     = 0xB0;
char 	data	PC_In_Buf_Index;
char 	data	PC_Out_Buf_Index;
char 	data	PC_In_Buf[PC_In_Buf_Max];
char	data	PC_Out_Buf[PC_Out_Buf_Max];
char	data	Indiv_To_PC[5];							// Individual data array to PC					
char 	data  	*ACUD_ID_3Dec;


/* ########## Declare related to ACP */
#define ACP_In_Buf_Max 			5				// ***** Need to be confirmed
#define ACP_Out_Buf_Max			5				// ***** Need to be confirmed
// Port 3: 
sbit 	ACP_RxD0		= P3^7;					// sbit RD   	= 0xB7;			// RD
sbit 	ACP_TxD0		= P3^6;					// sbit WR      = 0xB6;			// WR
sbit 	ACP_485Tx		= P3^5;					// sbit T1      = 0xB5;			// T1
sbit 	ACP_INT1		= P3^3;					// sbit INT1    = 0xB3;			// INT0, UART 
// related to Handler 
char 	data	Indiv_To_ACP_[5];						// Individual data array to ACP	
char 	data	ACP_In_Buf_Index;
char 	data	ACP_Out_Buf_Index;
char 	data	ACP_In_Buf[ACP_In_Buf_Max];
char 	data	ACP_Out_Buf[ACP_Out_Buf_Max];


/* ########## Declare related to ADC */
// ADC: SPI(Serial Peripheral Interface) Simulator
			 
// Port 1: 
sbit 	ADC_DIN_Pin  	= P1^3;
sbit 	ADC_SCLK_Pin 	= P1^2;
sbit 	ADC_CS_Pin   	= P1^1;
sbit 	ADC_DO_Pin		= P1^0;
int     data	ADC_Data;			// 2 bytes


/* ########## Declare related to ACUD */
// Port 0: 
sbit 	Fan_H_Pin	    = P0^0;					// Fan speed 
sbit 	Fan_M_Pin	 	= P0^1;					// Fan speed 
sbit 	Fan_L_Pin  		= P0^2;					// Fan speed 
sbit 	Air_Cooler_Pin	= P0^3;					// 
sbit 	Air_Heater_Pin	= P0^4;					// 
sbit 	Card_Det_Pin  	= P0^5;					// Card detection
// Port 3:
sbit 	WatchDog_ST  	= P3^4;					// sbit T0      = 0xB4;	        // T0
char	data	ACUD_ID_Hex;
float 	data	Temperature_Setting;
float	data	Temperature_Reality;
char	data	Checkout_Air_Period;			// 10-60min in an hour
char    data 	Minute_Counter;
char    data 	FAN_Speed;						// 0:L, 1:M,  2:H
//* union {										// union: all variables in union share same memory
	/* Note: data type must be signed or unsigned */
//*	unsigned char ACUD_Flag;
	struct {		
		unsigned char Card_Exist_Flg 		: 1; 	// 1: card existing, 0: card dispear	
		// unsigned char ACP_On_Flg			: 1; 	// 1: On, 0: Off
		unsigned char Air_Cool_Flg			: 1; 	// 1: Cooler, 0: eater
		unsigned char Air_Auto_Flg 			: 1; 	// 1: Auto mode, decide by ACP	
		unsigned char Card_Det_Flg 			: 1;	// Card detect request	
		unsigned char Temp_Rd_Flg			: 1;	// Temperautre reading request
		// unsigned char WD_Rst_Flg			: 1;	// WatchDog reset request
	}ACUD;
//* } ACUD;


/* Interrupt Vector(中斷向量)
		| INT Number |  Description  |	Address |
		|      0     |EXTERNAL INT  0|	 0003h  |   
	    |      1	 |TIMER/COUNTER 0|	 000Bh  |
	    |      2	 |EXTERNAL INT  1|	 0013h  |
		|	   3	 |TIMER/COUNTER 1|	 001Bh  |
		|      4	 |SERIAL PORT	 |   0023h  |
		|      5	 |TIMER/COUNTER 2|	 002Bh  | 
*/




// @@@@@@@@@@@@@@@      Program      @@@@@@@@@@@@@@@


// ########## Period Timer	
void TIMER0_Ten_mS_Init(char Ten){					// 10*mS timer

	TMOD &= 0xF0;								// Clear Timer 0 
	TMOD |= 0x01; 								// Mode 1, 16 bit timer/count mode	
	
	TH0 = (65536-(Ten*1000/1843200))/256;
	TL0 = (65536-(Ten*1000/1843200))%256;
	/* The content of TH0 & TL0 is designed to meet 1ms 
	TR0=1;;										// TCON.TR0=1, Timer 0 start running
	/* TCON: Related to Timer:
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		| TF1  | TR1  | TF0  | TR0  |      |      |      |      |
	TFx: Timer x OverFlow flag
		0 = Timer has not overflowed/rolled over
		1 = Timer has overflowed/rolled over	
	TRx: Timer 1/0 run control
		0 = Timer not running
		1 = Timer running|
	*/
	/* TCON: Related to External Interrupt: 
	
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		|      |      |      |      | IE1  | IT1  | IE0  | IT0  |
	IEx: External Interrupt(Int 0X13) Flag
		1 = Set by External Interrupt,when a high-to-low edge signal is received on port 3.3/3.2 (INT1/INT0)
		0 = Clear when processor vectors to interrupt service routine at program address 0013h. 
	ITx: External Interrupt Triger Control
		0: Set by program to enable external interrupt 1 to be triggered by a low-level signal to generate an interrupt.
		1: Set by program to enable external interrupt 1 to be triggered by a falling edge signal to generate an interrupt.
	*/
	Ten_mS_Counter = 0;
	
}
	
void TIMER0_Ten_mS() interrupt 1 {				// Timer0 INT vector=000Bh
	
	Ten_mS_Counter++;
	
	if(Ten_mS_Counter % 10 == 0){				// 100mS(10mS*10) period
		ACUD.Card_Det_Flg = 1;					// Room Card detect
		// ACUD.WD_Rst_Flg = 1;					// WatchDog reset
	}
	if(Ten_mS_Counter % 100 == 0){				// 1 Second(10mS*100) period

		ACUD.Temp_Rd_Flg = 1;					// Room Temperature detect
	}
	if(Ten_mS_Counter == 6000){					// 1 Minute
		Minute_Counter++;
		Ten_mS_Counter = 0;						// Reset 10mS_Counter
	}
}

void mS_Delay(char NmS){						// Delay N*ms 
	
	char  i;
	char  j;									// 宣告整數變數i,j
	for (i=0;i<NmS;i++)							// 計數N次,延遲 m*1ms 
	//#if Fosc == 22.1184						// 延遲 t*1ms @22.1184MHz
		for (j=0;j<1600;j++);
	//	;										// means NOP ;
	//#else										// 延遲 t*1ms @11.0952MHz
	//	for (j=0;j<800;j++);	 
	//#endif
}

void uS_Delay (char NuS){								// Delay u*us, 52us or 104us





	
}



// ########## PC Communication
// void PC_UART_Init(float Fosc ,unsigned short Baudrate){		// Include T1 init
void PC_UART_Init(){
	
	/* Special Function Registers Related to UART: 
		SCON		Serial Control Register
		TCON		Timer Control Register for Baud Rate Generator
		TMOD		Timer Mode Control for Baud Rate Generator
		TH1			
		SBUF		Serial Buffer holds the data to be transmitted and the data received 
	*/
	
	SCON = 0X40;									// SCON=01000000B, Mode 1 ,8 bit data and 1 stop bit, 
	REN = 1;										// CON=01010000B, Enable serial reception
	/* SCON: Serial Control Register
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		| SM0  | SM1  | SM2  | REN  | TB8  | RB8  |  TI  |  RI  |
	
	SM0-SM1: 
		00-Mode 0,	8-bit UART	Fosc/12 the quartz frequency
		01-Mode 1,	8-bit UART	Determined by the timer 1
		10-Mode 2,	9-bit UART	1/32 the quartz frequency
		11-Mode 3,	9-bit UART	Determined by the timer 1
	SM2 - 	Serial port mode 2 bit, also known as multiprocessor communication enable bit. 
			When set, it enables multiprocessor communication in mode 2 and 3, and eventually mode 1. 
			It should be cleared in mode 0.
	REN - 	Reception Enable bit enables serial reception when set. When cleared, serial reception is disabled.
	TB8 - 	Transmitter bit 8. Since all registers are 8-bit wide, this bit solves the problem of 
			transmitting the 9th bit in modes 2 and 3. It is set to transmit a logic 1 in the 9th bit.
	RB8 - 	Receiver bit 8 or the 9th bit received in modes 2 and 3. 
			Cleared by hardware if 9th bit received is a logic 0. Set by hardware if 9th bit received is a logic 1.
	TI 	- 	Transmit Interrupt flag is automatically set at the moment the last bit of one byte is sent. 
			It's a signal to the processor that the line is available for a new byte to transmit. 
			It must be cleared from within the software.
			傳送完畢旗號，此位元須自行清除,以便等待下一個TI=1中斷出現
	RI 	- 	Receive Interrupt flag is automatically set upon one-byte receive. 
			It signals that byte is received and should be read quickly prior to being replaced by a new data. 
			This bit is also cleared from within the software.
			接收完畢旗號，此位元須自行清除,以便等待下一個RI=1中斷出現
	*/

	TMOD &= 0x0F;									// Reset Timer 1 
	TMOD |= 0x20; 									// TMOD=00100000B, Timer1 in Mode 2, Auto reload mode.
	/* TMOD: Timer Mode Register:	
		| Tmler 1					| Timer 0                   |
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		| Gate | C/T  |  M1  |  M0  | Gate | C/T  |  M1  |  M0  |
			
	Gate Control
		0 = Timer enabled
		1 = Timer enabled if INTx is high
	C/T:Counter or Timer Selector
		0 = Internal count source (clock/12)
		1 = External count source (Tx pin)
	M1-M0: Mode Control
		00-Mode 0, 13 bit timer/count mode
		01-Mode 1, 16 bit timer/count mode
		10-Mode 2, Auto reload mode
		11-Mode 3, Multiple mode		
	*/

	TH1 = 256-(Fosc*1000000)/(long)(32*12*Baudrate); 	// SMOD=0
	// TH1 = 256-(Fosc*1000000)/(long)(16*12*Baudrate); // SMOD=1
	
	/* TH1 Value
		|  OSC MHz | Baud Rate |    SMOD   |Timer1 Mode|Timer1 Value|
		| 22.118MHz|   19200   |     0     |     2     |    0XFD    |   
		| 22.118MHz|   9600    |     0     |     2     |    0XFD    | 
		| 22.118MHz|   4800    |     0     |     2     |    0XFD    | 
		| 22.118MHz|   2400    |     0     |     2     |    0XFD    | 
		| 22.118MHz|   2400    |     0     |     2     |    0XFD    | 
		| 22.118MHz|   38400   |     1     |     2     |    0XFD    |   
		| 22.118MHz|   19200   |     1     |     2     |    0XFD    | 
		| 22.118MHz|   9600    |     1     |     2     |    0XFD    | 
		| 22.118MHz|   4800    |     1     |     2     |    0XFD    | 
		| 22.118MHz|   2400    |     1     |     2     |    0XFD    | 
		
		| 11.059MHz|   9600    |     0     |     2     |    0XFD    | 
		| 11.059MHz|   4800    |     0     |     2     |    0XFD    | 
		| 11.059MHz|   2400    |     0     |     2     |    0XFD    | 
		| 11.059MHz|   1200    |     0     |     2     |    0XFD    | 			
		| 11.059MHz|   19200   |     1     |     2     |    0XFD    | 
		| 11.059MHz|   9600    |     1     |     2     |    0XFD    | 
		| 11.059MHz|   4800    |     1     |     2     |    0XFD    | 
		| 11.059MHz|   2400    |     1     |     2     |    0XFD    | 	
	*/

	TR1=1;     									// TCON.TR1=1, Timer 1 start running
	/* TCON: Related to Timer:
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		| TF1  | TR1  | TF0  | TR0  |      |      |      |      |
	TFx: Timer x OverFlow flag
		0 = Timer has not overflowed/rolled over
		1 = Timer has overflowed/rolled over	
	TRx: Timer 1/0 run control
		0 = Timer not running
		1 = Timer running|
	*/
	/* TCON: Related to External Interrupt: 
	
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
		|      |      |      |      | IE1  | IT1  | IE0  | IT0  |
	ITx: External Interrupt Triger Control
		0: Set by program to enable external interrupt 1 to be triggered by a low-level signal to generate an interrupt.
		1: Set by program to enable external interrupt 1 to be triggered by a falling edge signal to generate an interrupt.
	IEx: External Interrupt(Int 0X13) Flag
		1 = Set by External Interrupt,when a high-to-low edge signal is received on port 3.3/3.2 (INT1/INT0)
		0 = Clear when processor vectors to interrupt service routine at program address 0013h. 
	
	*/
	
	PC_In_Buf_Index = 0;
	PC_Out_Buf_Index = 0;
	PC_485Tx = 0;								// RS485 Tx disable (= Rx enable)

}

bool PC_Tx_Handler(char *Indiv_PC_Tx_Ptr){		// Pointer of individual data array (Indiv_To_PC[])
	/* Data in Indiv_To_PC[] including "Enter" as tail */
	bool 	Resp;
	char 	i = 0;
	char 	PC_TBUF;
	
	if(!(Comm.PC_Tx_Busy_Flg)){				    // PC Tx avilable
	/* Got the rigth to allow data in Indiv_To_PC[] port to PC_Out_Buf[] */
	
		Comm.PC_Tx_Busy_Flg = 1; 				// clear by PC_UART_RxTx() interrupt 4
		PC_TBUF = *Indiv_PC_Tx_Ptr;
		while(PC_TBUF!= Enter){
			PC_Out_Buf[i]=*Indiv_PC_Tx_Ptr;
			i++;
			Indiv_PC_Tx_Ptr++;
		}
		PC_Out_Buf[i]=*Indiv_PC_Tx_Ptr;			// Put "Enter" into PC_Out_Buf[]
		PC_Out_Buf_Index = 0;					// Initial PC_Out_Buf_Index
		TI=1; 									// Triger UART_ISR() to start UART_Tx 
		
		Resp = true;
		return Resp;								// data transmit permit
	}
	else {
	/*  Indiv_To_PC[] does not to port to PC_Out_Buf[] */	
		Resp = false;
		return Resp;								// data transmit deny
	}
}

void PC_UART_RxTx() interrupt 4 { 				// UART INT, vector=0023h
	
	EA=0;										// Suspend all interrupt
	
	if ( RI ){ 									// SCON.RI, RI=1 means new content have received                       	
		if (SBUF != Enter) {					
		/* "Enter" emerged */
			PC_In_Buf[PC_In_Buf_Index] = SBUF;	
			PC_In_Buf_Index++;
			RI = 0;								// SCON.RI=0, force UART_Rx ready to receive again
		}
		else {									// PC_In_Buf_Index >= PC_In_Buf_Max 
			// PC_In_Buf[PC_In_Buf_Index] = SBUF;	
			/* "Enter" character not including in PC_In_Buf[] */
			PC_In_Buf_Index = 0;				// Reset PC_In_Buf_Index
			Comm.PC_Rx_Ready_Flg = 1; 			// For PC_StateEvent()
		}
	}

	if ( TI ){    								// SCON.TI, TI=1 means previous content have been sent.   
		/* PC_Out_Buf[] hab been prepared ready and call by PC_Tx_Handler(),
		  "Enter" char no neet to send to ACP */ 
		/* Comm.PC_Tx_Busy_Flg should been set in PC_Tx_Handler() */

		SBUF = PC_Out_Buf[PC_Out_Buf_Index];
		if ((SBUF != Enter)) {					
		/* "Enter" emerged */
			PC_485Tx = 1; 						// Enable RS485 Tx 
			PC_Out_Buf_Index++;
		}			
		else {									// UART Tx completed, UART_Outbuf_Index >= UART_Outbuf_Max 
			PC_Out_Buf_Index=0;					// Reset PC_Out_Buf_Index
			Comm.PC_Tx_Busy_Flg = 0;			// Clear UART Tx busy
			PC_485Tx = 0; 						// Disable RS485 Tx ( =Rx enable)
		}
		TI = 0;									// SCON.TI=0, UART_Tx ready to sent 
	}
	EA=1;										// Resume all interrupt
}		



// ########## ACP Communication
void ACP_Init(){

	ACP_In_Buf_Index = 0;	
	ACP_Out_Buf_Index = 0;
	ACP_485Tx = 0;								// RS485 Tx disable	(= Rx enable)
	IT1 = 1;									// Setting a high-to-low edge(Falling) signal method to triger EX1 interrupt.
	/* EX1 will be enabled in main() */
}

void ACP_Tx_PhyLayer(){									
	/* ACP_Out_Buf[] had been prepared ready and call by ACP_Tx_Handler(),
	   "Enter" char no neet to send to ACP */
	/* Comm.ACP_Tx_Busy_Flg had been set in ACP_Tx_Handler() */	
	char 	i = 0;
	char 	ACP_T_TEMP;

	ACP_T_TEMP = ACP_Out_Buf[ACP_Out_Buf_Index];
	while (ACP_T_TEMP != Enter){
		EA = 0;									// Suspending all interrupt happen 
		/* Interrupt disable for 1 byte period only */	
		
		ACP_485Tx = 1;							// RS485 Tx enable (= Rx disable)
		ACP_TxD0 = 0;							// sent Start bit "0" on P3.6(WR)
		uS_Delay(104);
		for (i=0;i<8; i++) {
			if (ACP_T_TEMP & 0x80) {
				ACP_TxD0 = 1;					// sent out "1"
			}else {
				ACP_TxD0 = 0;					// Sent out "0"
			}
			ACP_T_TEMP <<= 1; 					// ACP_T_TEMP left shift 1 bit 
			uS_Delay(104);
		}
		ACP_TxD0 = 1;							// sent Stop bit "1" on P3.6(WR)
		uS_Delay(104);	
		ACP_Out_Buf_Index++;
		ACP_T_TEMP = ACP_Out_Buf[ACP_Out_Buf_Index];
		
		EA = 1;									// Resume all interrupt
	}
	ACP_485Tx = 0;
	Comm.ACP_Tx_Busy_Flg = 0;				
	/* For ACP_Tx_Handler(), to allow transmit again over IOSerial */
}

bool ACP_Tx_Handler(char *Indiv_To_ACP_Ptr){		// Pointer of individual data array(Indiv_To_ACP[])
	/* Data in Indiv_To_ACP[] including "Enter" as tail */
	// sbit ACP_RxD0 	= P3^7;					// RD
	// sbit ACP_TxD0	= P3^6;					// WR
	// sbit ACP_485Tx   = P3^5;					// T1
	// sbit ACP_INT1 	= P3^3;					// INT1, connect to pin RxD0

	bool Resp;
	char 	i = 0;
	char 	ACP_T_TEMP;
	
	if(!(Comm.ACP_Tx_Busy_Flg)){				// IOSerial Tx avilable
	/* Got the right to allow data in Indiv_To_ACP[] port to ACP_Out_Buf[] */	
		Comm.ACP_Tx_Busy_Flg = 1; 				// clear by ACP_Tx()
		ACP_T_TEMP=*Indiv_To_ACP_Ptr;
		while(ACP_T_TEMP != Enter) {
			ACP_Out_Buf[i]=*Indiv_To_ACP_Ptr;
			i++;
			Indiv_To_ACP_Ptr++;
		}
		ACP_Out_Buf[i]=*Indiv_To_ACP_Ptr;		// Put "Enter" into ACP_Out_Buf[]
		ACP_Out_Buf_Index = 0;					// Initial PC_Out_Buf_Index
		ACP_Tx_PhyLayer();						// Call ACP_Tx_PhyLayer() to transmit data via ACP
		Resp = true;
		return Resp;								// data transmit permit
	}
	else {
		Resp = false;
		return Resp;								// data transmit deny
	}
}

void ACP_Rx() interrupt 2 {			
/* EX1 INT, vector=0013h, UART Simulator */
	
	char 	ACP_R_TEMP;							// Same as UART Rx SBUF
	// Tx no need to using interrupt. 
	// sbit ACP_RxD0 	= P3^7;					// RD
	// sbit ACP_TxD0	= P3^6;					// WR
	// sbit ACP_485Tx   = P3^5;					// T1
	// sbit ACP_INT1 	= P3^3;					// INT1, connect to pin RxD0
	
	EA = 0; 									// Suspending all interrupt happen 
	// EX = 0; 		
	/*   When IT1 set "0", EX1 need to be cleared to "0", after interrupt occured.
		 When IT1 set to "1" EX1 will be cleared to "0" by 8051, after interrupt occured.*/
	
	uS_Delay(52);
	if (ACP_RxD0 == 0){ 						// Expecting start bit "0" 
		char  i;
		for (i=0;i<8;i++){
			uS_Delay(104);
			if (ACP_RxD0 == 1){
				ACP_R_TEMP |= 1;
				ACP_R_TEMP <<= 1;				// ACP_SBUF left shift 1 bit 
			} else {
				ACP_R_TEMP <<= 1;				// ACP_SBUF left shift 1 bit 
			} 	
		}
		uS_Delay(104);					
		if (ACP_RxD0 != 1){ 				// Stop bit
		/* One byte(10 bits) received sucessful */
			if(ACP_R_TEMP != Enter ){
			/* String receiving not complete */
				ACP_In_Buf[ACP_In_Buf_Index] = ACP_R_TEMP;	
				ACP_In_Buf_Index++;
			}
			else {								// ACP_In_Buf_Index >= ACP_In_Buf_Max 
			/* String receiving completed */
				ACP_In_Buf[ACP_In_Buf_Index] = ACP_R_TEMP;
				/* "Enter" including in ACP_In_Buf[] */
				ACP_R_TEMP=0;
				ACP_In_Buf_Index = 0;			// Reset UART_Inbuf_Index
				Comm.ACP_Rx_Ready_Flg = 1; 		// For ACP_StateEvent()
			}
		}
	} else {
		/* One byte(10 bits) received failure */
		ACP_R_TEMP=0;
		ACP_In_Buf_Index = 0;					// Reset UART_Inbuf_Index
		Comm.ACP_Rx_Ready_Flg = 0; 				
	}
	EA = 1;										// Resume all interrupt
}


// ########## Temperture Reading: ADC AD7911
void ADC_Init(){
/* ADC AD7911 
// a minimum of 14 serial clock cycles, respectively, are needed 
// to complete the conversion and access the complete conversion result.
//
// DIN INPUT:
//	| bit13| bit12| bit11| bit10| bit9 | ---- | ---- | bit0 | 
//	|   X  |   X  |  CH  |   X  |        don't care         |
// The channel to be converted on in the next conversion is selected by 
// writing to the DIN pin.Only the third bit of the DIN word is used; 
// the rest are ignored by the ADC
//
// DIN OUTPUT:
//	| bit13| bit12| bit11| bit10| bit9 | ---- | ---- | bit0 | 
//	|   X  |   X  |  CH  |   X  | 10 bits conversion result |
// AD7911, the serial data stream consists of two leading zeros followed 
// by the bit that identifies the channel converted, an invalid bit that 
// matches up to the channel identifier bit, and the 10-bit conversion 
// result with MSB provided first, followed by two trailing zeros.
*/
	char 	i;
	ADC_SCLK_Pin = 1;
	ADC_CS_Pin = 1;
	ADC_CS_Pin = 0;

	if (i=0,i<14,i++){							
	/* Transmit 14(13-0) consecutive bit "0" */
		ADC_DIN_Pin = 0;						// Set bit 11(CH Select) = 0. CH0 select	
		ADC_SCLK_Pin = 0;
		ADC_SCLK_Pin = 1;
	}
	ADC_CS_Pin = 1;
}

float ADC_Rd(){									// n=10 or 12, n bits convert resolution
	char 	i;
	float 	ConvertedVolt = 0;
	
	ADC_DO_Pin = 1;
	ADC_CS_Pin = 1;
	ADC_SCLK_Pin = 0;
	if (i=0,i<14,i++){
		ADC_SCLK_Pin = 1;
		ADC_SCLK_Pin = 0;
		if (ADC_DO_Pin){
			ConvertedVolt = ConvertedVolt || 0x0001;		// set LSB = 1
		}else {
				ConvertedVolt = ConvertedVolt && 0xFFFE;   		// set LSB = 0
		}
		ADC_Data <<= 1 ;					// Left shift
	}
	ConvertedVolt = ConvertedVolt * 5 /(2^10 - 1);
	// Comm.ADC_Rd_Busy_Flg = 0;
	
	return ConvertedVolt;
}



// ########## ACUD

char Hex2Dec(char Hex2D){

	char 	Dec;
	char  remainder;
	char  count = 0;
	char 	decimal_number;
    while(Hex2D > 0){
		remainder = Hex2D % 10;
        decimal_number = Dec + remainder * pow(16, count);
        Hex2D = Hex2D / 10;
        count++;
    }
	return Dec;
}

char Dec2Hex(char Dec2H){

	char 	Hex;

	/* not impletement yet */

	return Hex;
}

char *HexInt2ASCIIStr(char HexInt2A){
	/* Return Function: A function which retrun a address */
	
	char 	AscStr[3];
	
	AscStr[0] = HexInt2A/100+48;					// Hunderd digit
	AscStr[1] = (HexInt2A/10)%10+48;				// Ten digit
	AscStr[2] = HexInt2A%10+48;					// digit
	
	return AscStr;
}

char *HexInt2DecStr(char HexInt2D){
	/* Return Function: A function which retrun a address */
	
	char 	DecStr[3];
	
	DecStr[0] = HexInt2D/100;						// Hunderd digit
	DecStr[1] = (HexInt2D/10)%10;					// Ten digit
	DecStr[2] = HexInt2D%10;						// digit
	
	return DecStr;
}

char DecStr2HexInt(char *DecStr){	
	/* string length must be 2 digits */

	char 	HexInt;
	char 	temp;
	
	HexInt = *DecStr;
	HexInt = (HexInt*10)/16;
	HexInt = HexInt << 1;									// Left rotate 
	temp = (HexInt*10)%16;							// 
	HexInt = HexInt + temp;
	temp = *(DecStr+1);
	HexInt = HexInt + temp;

	return HexInt;
}

void ACUD_Init(){
	
	P2 = 0xFF;
	ACUD_ID_Hex = P2;	
	Checkout_Air_Period = 10;					// 10-60min in an hour
	Minute_Counter = 0;
	//ACUD.Air_On_Flg = 0;
	ACUD.Air_Auto_Flg = 0;
	Temperature_Setting = 26;					// Checkout Default = 26 degree C
	Temperature_Reality = 26;
	FAN_Speed = 0; 								// 0:L, 1:M,  2:H
	
	Air_Cooler_Pin = 0;							// Turn cooler off		
	Air_Heater_Pin = 0; 						// Turn Herter off
	Fan_L_Pin = 0;								// Turn Fan Off
	Fan_M_Pin = 0;								
	Fan_H_Pin = 0;		
}


	
// ########## System Initialization
void System_Init(){
	
	PCON = 0x00;								// SMOD = 0;
	/* SMOD = 0;								// Baud rate selection
			 The PCON register is used for power control and double baud rate by set 1
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
		| SMOD | RSV2 |	RSV1 | RSV0	| GF1  | GF0  |	PWDN | IDL  |
	SMOD	The SMOD=1 is used to acquire twice high baud rate than SMOD=0
	RSV2	Reserve 
	RSV2	Reserve 
	RSV2	Reserve 
	GF1		一般用途位元，可當一個位元變數使用。
	GF0		一般用途位元，可當一個位元變數使用。
	PWDN	省電模式，必須使用reset訊號讓其回復到一般操作模式。
	IDL		IDL=1會使8051的clock停止，必須使用外部中斷或reset訊號使8051回復到一般操作模式。	
	*/
	
	/* P0(AIR) In/Out initial */
	
	/* P1(ADC) In/Out initial */
	
	/* P2(ACUD_ID_Hex) In/Out initial */
	
	/* P3(ACP) In/Out initial */
	
	PC_UART_Init();
	TIMER0_Ten_mS_Init(10);						// Timer0 10ms 
	ACP_Init();
	ADC_Init();
	ACUD_Init();
}

void PC_C_Event_Reply(char *Cmd){

	bool Resp;
	/* Reply back to PC */
	strcpy(Indiv_To_PC,"A");
	strcat(Indiv_To_PC,ACUD_ID_3Dec);
	strcat(Indiv_To_PC,Cmd);					
	strcat(Indiv_To_PC,Enter);						//	Carry Return
	/* "Enter" need to be included */
				
	Resp = PC_Tx_Handler(&Indiv_To_PC);
	if(Resp == true){
	/* anknowledge back to PC successful */
		PC_In_Buf[PC_In_Buf_Max] = 0;	
		Comm.PC_Rx_Ready_Flg = 0;
	}		
	else {
		/* anknowledge back to PC failure */	
		
		/* Ignore this failure. assuming command will be 
		   resend again by PC site */
		Comm.PC_Rx_Ready_Flg = 0;
	}
}

void PC_D_Event_Reply(){
	bool 	Resp;
	/* Reply back to PC */
	strcpy(Indiv_To_PC,"DONE");
	strcat(Indiv_To_PC,ACUD_ID_3Dec);					
	strcat(Indiv_To_PC,Enter);						//	Carry Return
	/* "Enter" need to be included */
				
	Resp = PC_Tx_Handler(&Indiv_To_PC);
	if(Resp == true){
	/* anknowledge back to PC successful */
		PC_In_Buf[PC_In_Buf_Max] = 0;	
		Comm.PC_Rx_Ready_Flg = 0;
	}		
	else {
		/* anknowledge back to PC failure */	
		
		/* Ignore this failure. assuming command will be 
		   resend again by PC site */
		Comm.PC_Rx_Ready_Flg = 0;
	}
}



// ########## Event manipulate 
/* PC Event manipulate */
void PC_StateEvent(){
	
	char *ACUD_ID_3Dec_Ptr;							// Point to ID start position
	char *Command_Ptr;								// point to Command start position
	char *Tempe_Ptr;								// Point to temperature start position
//	char	Indiv_To_PC[5];							// Individual data array to PC					
	/* Using array to instead of pointer to reserve memory firmdly, when implementing strcpy(), strcat() */

	if(Comm.PC_Rx_Ready_Flg){
		/* strstr(string1,string2)
		   This function returns a position points to the first character of the 
		   found s2 in s1, otherwise a null pointer.
		   if s2 is not present in s1, s1 is returned. 
		*/		
		ACUD_ID_3Dec = HexInt2DecStr(ACUD_ID_Hex);
		if(strstr(PC_In_Buf,ACUD_ID_3Dec)){			// String_Temp does occurre in PC_In_Buf 
		/* "Enter" character not including in PC_In_Buf[] */
				
			if(strstr(PC_In_Buf,"C")){				// "Command type" form PC
				
				ACUD_ID_3Dec_Ptr = strstr(PC_In_Buf,ACUD_ID_3Dec); 
				Command_Ptr = ACUD_ID_3Dec_Ptr+3 ;	// point to start position of Cmd

				if(strstr(Command_Ptr,"CI")) {		
				/* Card absent, Turn On Air */
					
					Minute_Counter = 0; 
					/* To impel air become on by Air_Manipulate() */
					
					Temperature_Setting = 23;		// Default=23			
					PC_C_Event_Reply(Command_Ptr);	// Reply same Cmd to PC which received from PC
				} 
				else if(strstr(Command_Ptr,"MO")) {	
				/* Card absent, Turn Off Air */
					
					Minute_Counter = Checkout_Air_Period; 
					/* To impel air become off by Air_Manipulate() */
					
					Temperature_Setting = 26;		// Default=23
					PC_C_Event_Reply(Command_Ptr);	// Reply same Cmd to PC which received from PC	
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"CO")) {	
				/* Card present, Turn On Air */
	
					ACUD.Air_Auto_Flg = 1;
					/* To impel air become auto by Air_Manipulate() */
					
					Temperature_Setting = 23;		// Default=23							
					PC_C_Event_Reply(Command_Ptr);	// Reply same Cmd to PC which received from PC
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"CC")) {	// Turn Off Cooler
						
					Minute_Counter = Checkout_Air_Period; 	
					/* To impel air become off by Air_Manipulate() */
					
					Temperature_Setting = 26;		// Default=23
					PC_C_Event_Reply(Command_Ptr);	
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"AC")) {	// Become Cooler mode
					
					/* No further action need */
					
					PC_C_Event_Reply(Command_Ptr);	// Reply same Cmd to PC which received from PC
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"AH")) {	// Become Heater mode
					
					/* No further action need */
					
					PC_C_Event_Reply(Command_Ptr);	// Reply same Cmd to PC which received from PC
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"ST")) {	// ST Temperature setting
					
					Tempe_Ptr = Command_Ptr + 2;	// point to start position of Temperature		
					Temperature_Setting = DecStr2HexInt(Tempe_Ptr);	
						
					PC_C_Event_Reply(Command_Ptr);	
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"IT")) {	// Key In Temperature setting
					
					Tempe_Ptr = Command_Ptr + 2;	// point to start position of Temperature		
					Temperature_Setting = DecStr2HexInt(Tempe_Ptr);	
						
					PC_C_Event_Reply(Command_Ptr);	
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"OT")) {	// Key Out Temperature setting
				
					Tempe_Ptr = Command_Ptr + 2;	// point to start position of Temperature		
					Temperature_Setting = DecStr2HexInt(Tempe_Ptr);	
					
					PC_C_Event_Reply(Command_Ptr);	
					/* Reply same Cmd to PC which received from PC*/
				}
				else if(strstr(Command_Ptr,"RU")) {	// Auto
					
					ACUD.Air_Auto_Flg = 1;
					Tempe_Ptr = Command_Ptr + 2; // point to start position of Temperature
					Checkout_Air_Period = DecStr2HexInt(Tempe_Ptr);
						
					PC_C_Event_Reply(Command_Ptr);	
					/* Reply same Cmd to PC which received from PC*/
				}
			}	
			else if ( strstr(PC_In_Buf,"DO")){		// "Confirm type" form PC
			
				// if(strstr(PC_In_Buf,Command_Ptr) {				 
						
					PC_D_Event_Reply();	
					/* Reply same Cmd to PC which received from PC*/
			}
		}
		/* ACUD_ID_3Dec is not match */
		PC_In_Buf[PC_In_Buf_Max] = 0;	
		Comm.PC_Rx_Ready_Flg = 0;
	}
}


/* ACP Event manipulate */

//	void ACP_StateEvent(){
//		//* Using array to instead of pointer to reserve memory firmdly, 
//		   when implementing strcpy(), strcat() */
//		int 	Resp;
//		
//		if(Comm.ACP_Rx_Ready_Flg){				
			
//			if (strstr(ACP_In_Buf,"command string 1") = 0) {
//			//* "Enter" was included in ACP_In_Buf  */	
					
				//* Reply acknowledge back to ACP */
					
//				Resp = ACP_Tx_Handler(&Indiv_To_ACP)
//				if (Resp == 1) {
//				//* anknowledge back to ACP successful */
//					ACP_In_Buf[ACP_In_Buf_Max] = {0};	
//					Comm.ACP_Rx_Ready_Flg = 0
//				}
//				else {
//				//* anknowledge back to ACP failure */	
//					Comm.ACP_Rx_Ready_Flg = 0;
//					//* Ignore this failure. assuming command will be resent again by PC site */
//				
//				}
//			}
//		}
//	}



void Air_Auto_Control(){

	float  Temperature_delta;

	Temperature_delta = Temperature_Setting-Temperature_Reality;
	/* Temperature_Reality will be updated by ADC_Rd() in IIMER0_NmS() interrupt 1
	   Temperature_Setting will be updated by PC_StateEvent() and ACP_StateEvent()  
	   
	// P0^2: FAN speed Low 
	// P0^3: FAN speed Middle
	// P0^4: FAN speed High 
	*/
	
	if(Temperature_delta > 3) {
		Air_Cooler_Pin = 1;						// Turn cooler on		
		Air_Heater_Pin = 0;						// Turn Herter off
		Fan_L_Pin = 0;								 
		Fan_M_Pin = 0;								
		Fan_H_Pin = 1;								 
	}
	else if(Temperature_delta > 2) {
		Air_Cooler_Pin = 1;						// Turn cooler on		
		Air_Heater_Pin = 0;						// Turn Herter off
		Fan_L_Pin = 0;								
		Fan_M_Pin = 1;								
		Fan_H_Pin = 0;
	}
	else if(Temperature_delta > 1) {
		Air_Cooler_Pin = 1;						// Turn cooler on		
		Air_Heater_Pin = 0;						// Turn Herter off
		Fan_L_Pin = 1;								
		Fan_M_Pin = 0;								
		Fan_H_Pin = 0;	
	}
	else if( Temperature_delta < 0.25 && Temperature_delta > -0.25 ) {
		Air_Cooler_Pin = 0;						// Turn cooler on		
		Air_Heater_Pin = 0;						// Turn Herter off
		Fan_L_Pin = 1;								
		Fan_M_Pin = 0;								
		Fan_H_Pin = 0;	
	}
	else if(Temperature_delta < -1 ) {
		Air_Cooler_Pin = 0;						// Turn cooler on		
		Air_Heater_Pin = 1;						// Turn Herter off
		Fan_L_Pin = 1;								
		Fan_M_Pin = 0;								
		Fan_H_Pin = 0;	
	}
	else if(Temperature_delta < -2 ) {
		Air_Cooler_Pin = 0;						// Turn cooler on		
		Air_Heater_Pin = 1;						// Turn Herter off
		Fan_L_Pin = 0;								
		Fan_M_Pin = 1;								
		Fan_H_Pin = 0;	
	}
	else if(Temperature_delta < -3 ) {
		Air_Cooler_Pin = 0;						// Turn cooler off		
		Air_Heater_Pin = 1;						// Turn Herter on
		Fan_L_Pin = 0;								
		Fan_M_Pin = 0;								
		Fan_H_Pin = 1;	
	}
}


void Air_Manual_Control(){
	
	if (ACUD.Air_Cool_Flg ) {
	/* Cooler */
		Air_Heater_Pin = 0;						// Turn Herter off
		if( Temperature_Reality > Temperature_Setting ){
			Air_Cooler_Pin = 1;					// Turn cooler on			
		}
		else {
			Air_Cooler_Pin = 0;					// Turn cooler off		
		}
		switch(FAN_Speed){
			case 0:
				Fan_L_Pin = 1;								
				Fan_M_Pin = 0;								
				Fan_H_Pin = 0;
			case 1:
				Fan_L_Pin = 0;								
				Fan_M_Pin = 1;								
				Fan_H_Pin = 0;
			case 2:
				Fan_L_Pin = 0;								
				Fan_M_Pin = 0;								
				Fan_H_Pin = 1;
			break;
		}	
	}	
	else {
	/* Heater */	
		Air_Cooler_Pin = 0;						// Turn cooler off		
		if( Temperature_Reality > Temperature_Setting ){
			Air_Heater_Pin = 0;					// Turn Herter off
		}
		else {
			Air_Heater_Pin = 1;					// Turn Herter off
		}

		switch(FAN_Speed){
			case 0:
				Fan_L_Pin = 1;								
				Fan_M_Pin = 0;								
				Fan_H_Pin = 0;
			case 1:
				Fan_L_Pin = 0;								
				Fan_M_Pin = 1;								
				Fan_H_Pin = 0;
			case 2:
				Fan_L_Pin = 0;								
				Fan_M_Pin = 0;								
				Fan_H_Pin = 1;
			break;
		}
	}
}


/* Aircondition manipulate */
void Air_Manipulate(){
	
	if(ACUD.Card_Exist_Flg){
	/* Card_In_Flg will be handled in IIMER0_NmS() interrupt 1 */
		
		/* Card present */	
		if(ACUD.Air_Auto_Flg){
		/* Performing Auto Mode */
			Air_Auto_Control();
		}
		else {
		/* Performing Manual Mode */
			Air_Manual_Control();
		}
	}
	else {	
	/* Card absent */
		if(Minute_Counter < Checkout_Air_Period){
			Air_Auto_Control();
		} 
		else if(Minute_Counter < 60){
			Air_Cooler_Pin = 0;					// Turn cooler off		
			Air_Heater_Pin = 0; 				// Turn Herter off
			Fan_L_Pin = 0;						// Turn Fan off
			Fan_M_Pin = 0;								
			Fan_H_Pin = 0;								
		} 
		else {
				Minute_Counter = 0;
		}
	}
}	


/* Watchdog */
void WatchDog(){
	
		WatchDog_ST =~ WatchDog_ST;				// complement, to produce 10101010....
	
	/* The ST input can be derived from microprocessor address signals, 
	   data signals, and/or control signals. When the microprocessor is 
	   functioning normally, these signals would, as a matter of routine,
	   cause the watchdog to be reset prior to timeout. To guarantee
	   that the watchdog timer does not timeout, a high-to-low transition
	   must occur at or less than the minimum shown in Table
		| TD PIN	|	MIN 	|	TYP 	|	MAX		|
		| GND 		|	62.5 ms |	150 ms 	|	250 ms	|
		| Float 	|	250 ms 	|	600 ms 	|	1000 ms	|
		| VCC 		|	500 ms 	|	1200 ms |	2000 ms	|
		Invalue ST > 20nS   
	*/
	
}



// ########## Main Program ##########
main(){

	IE = 0x00;									// Set all interrupt disable
	
	/* Initialization manipulate */
	System_Init();	
	
	/* Interrupt Priority: 	IP		// Put this function being as last function 
	| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 	
	| RSV  | RSV  |	PT2	 | PS	| PT1  | PX1  | PT0	 | PX0  |
	RSV	Reserve
	RSV	Reserve
	PT2	Timer2 
	PS	Serial
	PT1	Timer1 
	PX1	External 1 
	PT0	Timer0 
	PX0	External 0 
	*/
	IP=0x00;									// Default priority						
	/* Interrupt Enable: 	IE
	| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 | 
	|  EA  | RSV  |	ET2  |	ES	| ET1  | EX1  |	ET0	 | EX0  |
	EA	Enable
	RSV	Reserve
	ET2	Timer 2 
	ES	Serial 
	ET1	Timer 1
	EX1	Ecternal 1
	ET0	Timer 0
	EX0	External 0	
	*/
	ES=1;										// IE.ES,  Enable Serial Interrupt, UART comm. with PC
	ET0=1;										// IE.ET0, Enable Timer0 Interrupt, 10ms period
	EX1=1;										// IE.EX1, Enable Ext 1 Interrupt, Serial comm. with ACP
	EA=1;										// IE.EA,  Enable all interrupt							

	while(1){
	
		if(ACUD.Card_Det_Flg){					// Set by IIMER0_10mS() interrupt 1
			ACUD.Card_Det_Flg = 0;				// Clear flag
			ACUD.Card_Exist_Flg = Card_Det_Pin;	// Reading Key exist status
		}	
		if(ACUD.Temp_Rd_Flg){					// Set by IIMER0_10mS() interrupt 1
			ACUD.Temp_Rd_Flg = 0;				// Clear flag
			Temperature_Reality = ADC_Rd();		// Implement ADC reading
		}

		PC_StateEvent();
		// ACP_StateEvent();
		// ACUD_StateEvent();
		Air_Manipulate();						// depend on the command in Fan_Status 
		
		WatchDog();
		
	}
}
