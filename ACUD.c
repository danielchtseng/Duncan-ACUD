
// 8051 Keil C 
// ACUD 
// Auther: Duncan Tseng
// Ver: W053-H1600

// 485Tx_ACP need to be implemented, Handler need to be rechecked

// @@@@@@@@@@ Daclare @@@@@@@@@@

#include <reg51.h>

/* BYTE Register
sfr P0   = 0x80;
sfr P1   = 0x90;
sfr P2   = 0xA0;
sfr P3   = 0xB0;
sfr PSW  = 0xD0;
sfr ACC  = 0xE0;
sfr B    = 0xF0;
sfr SP   = 0x81;
sfr DPL  = 0x82;
sfr DPH  = 0x83;
sfr PCON = 0x87;
sfr TCON = 0x88;
sfr TMOD = 0x89;
sfr TL0  = 0x8A;
sfr TL1  = 0x8B;
sfr TH0  = 0x8C;
sfr TH1  = 0x8D;
sfr IE   = 0xA8;
sfr IP   = 0xB8;
sfr SCON = 0x98;
sfr SBUF = 0x99;
*/

/* SPF BIT 
// PSW   
sbit CY   	= 0xD7;
sbit AC   	= 0xD6;
sbit F0   	= 0xD5;
sbit RS1  	= 0xD4;
sbit RS0  	= 0xD3;
sbit OV   	= 0xD2;
sbit P    	= 0xD0;

// Port 1: 
// SPI(Serial Peripheral Interface) Simulator
sbit 			SPI_DO  	= P1^0;
sbit 			SPI_CS   	= P1^1;
sbit 			SPI_SCLK 	= P1^2;
sbit 			SPI_DIN  	= P1^3;

// Port 3: 
// Serial Simulator
sbit Serial_RXD0	= P3^7;		// sbit RD   	= 0xB7;			// RD
sbit Serial_TXD0	= P3^6;		// sbit WR      = 0xB6;			// WR
sbit 485Tx_ACP		= P3^5;		// sbit T1      = 0xB5;			// T1
sbit INT1_Serial	= P3^3;		// sbit INT1    = 0xB3;			// INT0, UART 						
// UART
sbit 485Tx_PC   	= P3^2;		// sbit INT0    = 0xB2;			// UART TXD
sbit UART_TXD1 		= P3^1;		// sbit TXD     = 0xB1;			// UARD RXD
sbit UART_RXD1 		= P3^0;		// sbit RXD     = 0xB0;

// TCON  
sbit TF1  	= 0x8F;
sbit TR1  	= 0x8E;
sbit TF0  	= 0x8D;
sbit TR0  	= 0x8C;
sbit IE1  	= 0x8B;
sbit IT1  	= 0x8A;
sbit IE0  	= 0x89;
sbit IT0  	= 0x88;

// SCON  
sbit SM0  	= 0x9F;
sbit SM1  	= 0x9E;
sbit SM2  	= 0x9D;
sbit REN  	= 0x9C;
sbit TB8  	= 0x9B;
sbit RB8  	= 0x9A;
sbit TI   	= 0x99;
sbit RI   	= 0x98;

// IP   
sbit PS   	= 0xBC;					// sbit PS  = 0xB8^4
sbit PT1  	= 0xBB;					// sbit PT1 = 0xB8^3
sbit PX1  	= 0xBA;					// sbit PX1 = 0xB8^2
sbit PT0  	= 0xB9;					// sbit PT0 = 0xB8^1
sbit PX0  	= 0xB8;					// sbit PX0 = 0xB8^0

// IE   
sbit EA   	= 0xAF;					// sbit EA  = 0xA8^7
sbit ES   	= 0xAC;					// sbit ES  = 0xA8^4
sbit ET1  	= 0xAB;					// sbit ET1 = 0xA8^3
sbit EX1  	= 0xAA;					// sbit EX1 = 0xA8^2
sbit ET0  	= 0xA9;					// sbit ET0 = 0xA8^1
sbit EX0  	= 0xA8;					// sbit EX0 = 0xA8^0
*/



// ***** Declare related to Communication
union Bit_Field {					// all variables in union share same memory
	
	unsigned Flag;
	Struct {		// 注意: type 必須為整數(signed or unsigned皆可)
		// relate to PC
		unsigned UART_Tx_Busy_Flg 		: 1;			// UART transmitting		
		unsigned PC_Rx_Appeared_Flg 	: 1;
		unsigned PC_Tx_Pending_Flg 		: 1;
		// relate to ACP
		unsigned IOSerial_Tx_Busy_Flg 	: 1;		
		unsigned ACP_Rx_Appeared_Flg 	: 1;		
		unsigned ACP_Tx_Pending_Flg 	: 1;
		// relate to ADC
		unsigned Rd_ADC_Busy_Flg 		: 1;
	}
};
/* Bit Field: 是一種省空間的特殊 data member, 可以使用特定幾個 bit 來放 data.
// The declaration of a bit-field has the following form inside a structure
// 		struct {
//   		type [data member] : width ;
// 		};
//
// For example
//      struct {
//			unsigned int age : 1;
//		} Flag;   						*/

// Declare related to UART 		
#define Fosc				22.1184	// 0r 11.0592 Need to be confirmed
#define Baudrate			9600	// ***** Need to be confirmed
#define UART_In_Buf_Max 		5;	// ***** Need to be confirmed
#define UART_Out_Buf_Max		5;	// ***** Need to be confirmed
int 	UART_In_Buf_Index;
int 	UART_Out_buf_Index;
char 	UART_In_Buf[UART_In_Buf_Max];
char 	UART_Out_Buf[UART_Out_Buf_Max];
//int 	*UART_Tx_Data_Ptr_Temp;
//int		UART_Tx_Data_Len_Temp;

// Declare related to ACP
// related to IOSerial 
int 	ACP_RBUF;
int 	ACP_TBUF;
// related to Handler 
int 	ACP_In_Buf_Index;
int 	ACP_Out_Buf_Index;
char 	ACP_In_Buf[ACP_In_Buf_Max];
char 	ACP_Out_Buf[ACP_Out_Buf_Max];
#define ACP_In_Buf_Max 		5;		// ***** Need to be confirmed
#define ACP_Out_Buf_Max		5;		// ***** Need to be confirmed



// ***** Declare related to ADC

unsigned int   	SPI_ConvertedData	// 2 bytes
int 			ConvertedData		// 


// ***** Declare related to ACUD
int 	ACUD_ID	





// @@@@@@@@@@ Program @@@@@@@@@@.


// ##### Period 	
/* Interrupt Vector(中斷向量)
		| INT Number |  Description  |	Address |
		|      0     | EXTERNAL INT0 |	 0003h  |   
	    |      1	 |TIMER/COUNTER 0|	 000Bh  |
	    |      2	 | EXTERNAL INT 1|	 0013h  |
		|	   3	 |TIMER/COUNTER 1|	 001Bh  |
		|      4	 |  SERIAL PORT	 |   0023h  |
		|      5	 |TIMER/COUNTER 2|	 002Bh  | */
	
void IIMER0_NmS() interrupt 1 {		// Timer0 INT vector=000Bh
	
	Key_Detect();					// Udate Key present status
	ADC_Detect();					// Update Temperture status
	

//	if ( FLAG.PC_Tx_Pending_Flg==1){
//		PC_Tx_Handler(UART_Tx_Data_Ptr_Temp, UART_Tx_Data_Len_Temp);	
//	}
//	if ( FLAG.ACP_Tx_Pending_Flg==1){
//		PC_Tx_Handler(ACP_Tx_Data_Ptr_Temp, ACP_Tx_Data_Len_Temp);
//	}
}

void mS_Delay(int m){				//	Delay ms 
	
	int i,j;						// 宣告整數變數i,j,N 
	
	for (i=0;i<m;i++)				// 計數N次,延遲 N*1ms 
	#if Fosc==22.1184				// 延遲 t*1ms @22.1184MHz
		for (j=0;j<1600;j++);	
	#else							// 延遲 t*1ms @11.0952MHz
		for (j=0;j<800;j++);	 
	#endif
}

void uS_Delay (int u){				// Delay us, 52us or 104us





	
}



// ##### PC TxRx
void PC_UART_Init(float Fosc ,int Baudrate){	// include T1 init
	
	/* Special Function Registers Related to UART: 
		SCON		Serial Control Register
		TCON		Timer Control Register for Baud Rate Generator
		TMOD		Timer Mode Control for Baud Rate Generator
		TH1			
		SBUF		Serial Buffer holds the data to be transmitted and the data received 
	*/
	
	SCON=0X40;						// SCON=01000000B, Mode 1 ,8 bit data and 1 stop bit, 
	REN=1;							// SCON=01010000B, Enable serial reception
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

	TMOD&=0x0F						// Reset Timer 1 
	TMOD|=0x20; 					// TMOD=00100000B, Timer1 in Mode 2, Auto reload mode.
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

	TH1 = 256-(Fosc*1000000)/(long)(32*12*Baudrate); // Load timer value for baudrate generation
	/* TH1 Value
		|  OSC MHz | Baud Rate |    SMOD   |Timer1 Mode|Timer1 Value|
		| 22.118MHz|   38400   |     1     |     2     |    0XFD    |   
		| 22.118MHz|   19200   |     1     |     2     |    0XFD    | 
		| 22.118MHz|   9600    |     1     |     2     |    0XFD    | 
		| 22.118MHz|   4800    |     1     |     2     |    0XFD    | 
		| 22.118MHz|   2400    |     1     |     2     |    0XFD    | 
		
		| 11.059MHz|   19200   |     1     |     2     |    0XFD    | 
		| 11.059MHz|   9600    |     1     |     2     |    0XFD    | 
		| 11.059MHz|   4800    |     1     |     2     |    0XFD    | 
		| 11.059MHz|   2400    |     1     |     2     |    0XFD    | 	
	*/

	TR1=1;     						// TCON.TR1=1, Timer 1 start running
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
	
	UART_In_Buf_Index = 0;
	UART_Out_Buf_Index = 0;
	DE1_PC = 0;						// RX485 Rx enable
}

void PC_Tx_Handler(int *Tx_Data_Ptr, int Len){
	
	int i;
	// data need to be port to UART_Out_Buf[] before by way of UART
	if(!(FLAG.UART_TX_Busy_Flg) == 1){// UART Tx avilable
	
		Flag.UART_TX_Busy_Flg = 1; 

		for (i=0,i<Len,i++) {
			UART_Out_Buf[i]=*Tx_Data_Ptr;
			Tx_Data_Ptr++;
		}
		UART_Out_Buf_Index = 0;		// Initial UART_Out_Buf_Index
		//FLAG.PC_Tx_Pending_Flg = 0;
		TI=1; 						// Triger UART_ISR() to start UART_Tx 
		
		return 1;
	}
	else {
		
		return 0;
		//UART_Tx_Data_Ptr_Temp=Tx_Data_Ptr;
		//UART_Tx_Data_Len_Temp=Len;
		//FLAG.PC_Tx_Pending_Flg = 1;// Handover to ISR TIMER0_NmS() 
	}
}

void PC_UART_RxTx() interrupt 4 { 	// UART INT, vector=0023h
	
	EA=0;							// Suspend all interrupt
	
	if ( RI ){ 						// SCON.RI, RI=1 means new content have received                       	
		
		if( UART_In_Buf_Index < UART_In_Buf_Max){
			UART_In_Buf[UART_In_Buf_Index] = SBUF;	
			UART_In_Buf_Index++;
			RI = 0;					// SCON.RI=0, force UART_Rx ready to receive again
		}
		else {	// (UART_Inbuf_Index >= UART_Inbuf_Max 

				UART_In_Buf_Index = 0;			// Reset UART_Inbuf_Index
				Flag.PC_Rx_Appeared_Flg = 1; 	// 
		}
	}

	if ( TI ){    		// SCON.TI, TI=1 means previous content have been sent.   
		
		// Flag.UART_TX_Busy_Flg had been set in PC_Tx_Handler()		
		
		if ( UART_Out_Buf_Index < UART_Out_Buf_Max ){
			485Tx_PC = 1; 			// T1, RX485 Tx enable
			SBUF = UART_Out_Buf[UART_Out_Buf_Index];
			UART_Out_Buf_Index++;
			TI = 0;					// SCON.TI=0, force UART_Tx ready to sent again
		}		
		else {						// UART Tx completed, UART_Outbuf_Index >= UART_Outbuf_Max 
	
			UART_Out_Buf_Index=0;		// Reset UART_Inbuf_Index
			
			Flag.UART_TX_Busy_Flg = 0;	// UART Tx busy
			
			485Tx_PC = 0; 				// T1, RX485 Tx Disable (=Rx enable)
		}	
	}
	
	EA=1;							// Resume all interrupt
}		



// ##### ACP TxRx
void ACP_IOSerial_Init(){
	
	Serial_TXD0 = 1;				// Initial "1" on P3.6(WR)
	FLAG.ACP_Tx_Pending_Flg = 0;
}

int ACP_Tx_Handler(int *Tx_Data_Ptr, int Len){

	// sbit Serial_RXD0 = P3^7;		// RD
	// sbit Serial_TXD0	= P3^6;		// WR
	// sbit 485Tx_ACP   = P3^5;		// T1
	// sbit INT1_Serial = P3^3;		// INT1, connect to pin RXD0

	// data need to be port to UART_Out_Buf[] before by way of UART
	if(!(FLAG.IOSerial_TX_Busy_Flg) == 1){// IOSerial Tx avilable
	
		Flag.IOSerial_TX_Busy_Flg = 1; 

		for (i=0,i<Len,i++) {
			ACP_Out_Buf[i]=*Tx_Data_Ptr;
			Tx_Data_Ptr++;
		}
		ACP_Out_Buf_Index = 0;		// Initial UART_Out_Buf_Index
		// FLAG.ACP_Tx_Pending_Flg = 0;
		
		ACP_IOSerial_Tx();			// Call IOSerial_Tx_ACP() to transmit data via ACP
		
		
		return 1;	
	}
	else {
		return 0;
		//ACP_Tx_Data_Ptr_Temp=Tx_Data_Ptr;
		//ACP_Tx_Data_Len_Temp=Len;
		//FLAG.ACP_Tx_Pending_Flg = 1;	// Handover to ISR TIMER0_NmS() 
	}

}

void ACP_IOSerial_Tx(){				// Call by ACP_Tx_Handler()
	
	int i;
	
	for (ACP_Out_Buf_Index=0;i<ACP_Out_Buf_Max,ACP_Out_Buf_Index++) {
		
		ACP_TBUF = ACP_Out_Buf[ACP_Out_Buf_Index];
		EA = 0;						// Suspending all interrupt happen during every single byte transmitting
		Serial_TXD0 = 0;			// sent Start bit "0" on P3.6(WR)
		uS_Delay(104);
		for (i=0;i<8; i++) {
			if (ACP_TBUF & 0x80) {
				Serial_TXD0 = 1;	// sent out "1"
			}else {
				Serial_TXD0 = 0;	// Sent out "0"
			}
			ACP_TBUF <<= 1; 			// ACP_TBUF left shift 1 bit 
			uS_Delay(104);
		}
		Serial_TXD0 = 1;			// sent Stop bit "1" on P3.6(WR)
		uS_Delay(104);				// 
		Flag.IOSerial_TX_Busy_Flg = 0;
		EA = 1;						// Resume all interrupt
	}
}

void ACP_IOSerial_Rx() interrupt 2 {// EXT1 INT, vector=0013h, UART Simulator
									// Tx no need to using interrupt. 
	// sbit Serial_RXD0 = P3^7;		// RD
	// sbit Serial_TXD0	= P3^6;		// WR
	// sbit 485Tx_ACP   = P3^5;		// T1
	// sbit INT1_Serial = P3^3;		// INT1, connect to pin RXD0
	
	uS_Delay(52);
	
	for (i=0;i<8,i++) {
		
		if (Serial_RXD0 == 1) {
			ACP_RBUF |= 1;
			ACP_RBUF << 1;				//ACP_SBUF left shift 1 bit 
			uS_Delay(104);
		} else
		Flag.ACP_Rx_Appeared_Flg = 1;
	} 
}



// ##### Read ADC AD7911
void ADC_SPI_Init(){
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
	int i;
	SPI_SCLK = 1;
	SPI_CS = 1;
	SPI_CS = 0;

	if (i=0;i<14;i++){	// Transmit 14(13-0) bit
		SPI_DIN = 0;	// bit 11 = 0, select CH = 0	
		SPI_SCLK = 0;
		SPI_SCLK = 1	
	}
	SPI_CS = 1;
	Flag.Rd_ADC_Busy_Flg = 0;
}

Rd_ADC( ){				// n=10 or 12, n bits convert resolution
	int i;
	float CovertedVolt = 0;
	
	if (!(Flag.Rd_ADC_Busy_Flg)) {
		Flag.Rd_ADC_Busy_Flg = 1;
		SPI_DO = 1;
		SPI_CS = 1;
		SPI_SCLK = 0;

		if (i=0;i<14;i++){
			SPI_SCLK = 1;
			SPI_SCLK = 0;
			if (SPI_DO){
				ConvertedVolt |= 0x0001);	// set LSB = 1
			}else {
				ConvertedVolt &= 0xFFFE;   		// set LSB = 0
			}
			SPI_Data <<= 1 ;
		}
	
		ConvertedVolt = ConvertedVolt * 5 / (2 ^ 10 - 1);
		Flag.Rd_ADC_Busy_Flg = 0;
		return CovertedVolt;
	}
	else {
		
	}
}



// ##### Timer

void TIMER0_NmS_Init(int N){		// NmS timer

	TMOD&=0xF0;						// Clear Timer 0 
	TMOD|=0x01; 					// Mode 1, 16 bit timer/count mode	
	TH0=(65536-(N*1000/(Fosc*1000000/12)))/256;
	TL0=(65536-(N*1000/(Fosc*1000000/12))%256;	
	TR0=1;;							// TCON.TR0=1, Timer 0 start running
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
}
	ACUD_ID = P2;					// 
	
}

// ##### Initialization
void System_Init(){
	
	PCON=0x00;
	SMOD=0;							// Baud rate selection
	/* PCON: Power Control Register: 
			 The PCON register is used for power control and double baud rate by set 1
		| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
		| MOD  | RSV2 |	RSV1 | RSV0	| GF1  | GF0  |	PWDN | IDL  |
	SMOD	The SMOD bit is used to decide the baud rate in serial port operating modes 1, 2 or 3.
	RSV2	Reserve 
	RSV2	Reserve 
	RSV2	Reserve 
	GF1		一般用途位元，可當一個位元變數使用。
	GF0		一般用途位元，可當一個位元變數使用。
	PWDN	省電模式，必須使用reset訊號讓其回復到一般操作模式。
	IDL		IDL=1會使8051的clock停止，必須使用外部中斷或reset訊號使8051回復到一般操作模式。	
	*/
	
	PC_UART_Init(22.1184,9600);
	ACP_IOSerial_Init();
	ADC_SPI_Init();
	TIMER0_NmS_Init(20);			// Timer0 20ms 
	ACUD_Init();
	
}



// ##### System
void ACUD_Init(){






// ##### Main Program #####

Main() {

	IE=0x00;						// Set all interrupt disable
	
	// ***** Initialization manipulate
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
	PX0	External 0 */
	IP=0x00;						// Default priority
							
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
	EX0	External 0	*/
	ES=1;							// IE.ES,  Enable Serial Interrupt, UART comm. with PC
	ET0=1;							// IE.ET0, Enable Timer0 Interrupt, 10ms period
	EX1=1;							// IE.EX1, Enable Ext 1 Interrupt, Serial comm. with ACP
	EA=1;							// IE.EA,  Enable all interrupt							

	while(1){
	
		PC_StateEvent();
		ACP_StateEvent();
		ACUD_StateEvent();

		Air_Manipulate();			// depend on the command in Fan_Status 
	}
}


// ##### Event manipulate 
// PC Event manipulate
void PC_StateEvent(){

	while(Flag.PC_Rx_Appeared_Flg){

		if ( Strcpm ( UART_In_Buf," string ") = 0) {



			break;
		} else if Strcpm ( UART_In_Buf," string ") = 0){
			
			
			
			break;
		} else if Strcpm ( UART_In_Buf," string ") = 0){
			
			
			
			break;
		} else if Strcpm ( UART_In_Buf," string ") = 0){	
		
		
		
			break;
		} else if Strcpm ( UART_In_Buf," string ") = 0){
			
		
			break;
		}
		Flag.PC_Rx_Appeared_Flg = 0
	}
}

// ACP Event manipulate
void ACP_StateEvent(){

	while(Flag.ACP_Rx_Appeared_Flg){

		if ( Strcpm ( ACP_In_Buf," string ") = 0) {


		
			break;
		}
		if ( Strcpm ( ACP_In_Buf," string ") = 0) {



			break;
		}
		if ( Strcpm ( ACP_In_Buf," string ") = 0) {



			break;
		}
		if ( Strcpm ( ACP_In_Buf," string ") = 0) {



			break;
		}
		
		Flag.ACP_Rx_Appeared_Flg = 0
	}
}

// ACUD Event manipulate
void ACUD_StateEvent(){
	
	
	

}

// Aircondition manipulate
void Air_Manipulate(){
	Tempeture_Detect();		// depend on the command in Temo_Status 
	Fan_Adjust();			// depend on the command in Fan_Status 
}
