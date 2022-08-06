#include <reg51.h>
#include <stdio.h>
#define output P1

// Declaring Data Bus and Control Bus 
sbit OE=P2^2;
sbit SC=P2^1;
sbit EOC=P2^3;
sbit ALE=P2^0;
//ADC Input Channels
sbit In_A=P2^5;
sbit In_B=P2^6;
sbit In_C=P2^7;

sbit WindingA=P3^4;   // For Stepper Motor
sbit WindingB=P3^5;
sbit WindingC=P3^6;
sbit WindingD=P3^7;
sbit Load = P3^3;    // For Load

// User Defined Function Prototyping
unsigned char TempADC ();
void TempConvert(unsigned char);
unsigned char PressureADC ();
void PressureConvert(unsigned char);
unsigned char VoltageADC();
void VoltageConvert(unsigned char);
void TempAuto(unsigned char);
void PressureAuto(unsigned char);
void delay (unsigned int);
void Timerdelay();

void SerialT () interrupt 4          // Using IE.4 which Serial Interrupt
{
	unsigned char value; 
	if (RI==1)    										// Whenever transmitted dataa is received it RI is High
	{
	value = SBUF;   									// Store dataa in variable 'value' from SBUF
	if (value =='T')    							// When 'T' received, it means Master needs Temperature dataa
	{
		TempConvert(TempADC());   			// Calling the function and converts dataa for it
	}
	if (value =='P')									// When 'P' received, it means Master needs Pressure dataa
	{
		PressureConvert(PressureADC());	// Calling the function and converts dataa for it
	}
	if (value =='V')									// When 'V' received, it means Master needs Voltage dataa
	{
		VoltageConvert(VoltageADC());		// Calling the function and converts dataa for it
	}
}
	RI=0;  // Clear Receive interupt , it is over go out from ISR
}
void Timerdelay()    // Timer delay of 71ms 
{
	TH0 = 0x00;          // Using 16 bits timer0 in mode 1
	TL0 = 0x00;
	TR0 = 1;					 // Start Timer 0 for timer delay
	while (TF0==0);    // Minotor this Flag, when overflow means Raised delay of 71 ms completes 
	TF0=0;						 // forced to zero the TF0, for next alteration
}
// Main Function
void main(void)
 { 
	P1=0xFF;  		 // Input pin because ADC gives digital dataa as input to Slave
	TMOD = 0x21;   // Both timer 0 and 1 uses, timer 0 for Baud rate, timer 1 for Timerdelay of Mode2 which is 16 bits
	TH1 = 0xFD;    // Baud rate is 28.8k/3 = 9600 = 0xFD in hex
	SCON = 0x50;   // 01010000 means SMO=0, SM1=1 which is Mode 2, 8 bits with start and stop bits 
	Load=0;        // Initially, Load is OFF
	Timerdelay();  // Calling the timerdelay of 71ms
	
	// Write your codes for ADC0808
	EOC=1;  				// declare as Input to Microcontroller
	ALE=0;   				// Clear Address Latch Enable
	OE=0;    				// Clear Output Enable
	SC=0;    				// Clear SC 		
	 
	IE=0x90; 				//10010000 , EN=1 and ES=1 for Serial Communication
	TR1 = 1; 				// Start Timer 1 for Baud rate
	while(1) 				// Forever Loop
	{
	  TempAuto(TempADC());         // Auto-updates the output value of Temperature for device control
	  PressureAuto(PressureADC()); // Auto-updates the output value of Pressure for device control
  }
}
void TempAuto(unsigned char tempvalue)  // Function for autoupdates the value for device control
{
	unsigned char tenth; 
	
	tempvalue = tempvalue*1.99;   // ADC output value converts into Temperature by formula
			
	tenth = tempvalue/10;         // Divide by 10 to get the ten value
	
	if (tenth>=4)    							// Temp is greater than 40
	 { 
		 Load=1;										// Turn ON AC or Load
	 }
	 else													// Temp is less than 40
	 {
		 Load=0;										// Turn OFF AC or Load
	 }
}	

void PressureAuto(unsigned char pressure)
{
	// P1=output, after conversion, hex or dec value is stored in Output that is P1. Now, we can read it
	unsigned char tenth;
	float vout;
 	unsigned char pressurevalue; 
		
	vout = (5.0/256)* pressure;   				       // 256 due to 8 bits converter, gets the output voltage
	pressurevalue = ((vout/5.0)+0.09)/0.009-1;   // Convert Voltage to Pressure (KPA)
			
	tenth = pressurevalue/10;            				 // For Ten or MSB value we divide by 10 
	
	if (tenth<4)		// Pressure MSB value is less than 4
	 {
		 WindingA=1;   WindingB=0;	WindingC=0;	 WindingD=1;    // Stepper Motor will act as stationary at 0 degree
	 }
	 if (tenth==4)
	 {
		 WindingA=0;   WindingB=0;  WindingC=0;  WindingD=1;		// Stepper Motor will rotate at 45 degree
	 }
	 if (tenth==5)
	 {
		 WindingA=0;   WindingB=0;	WindingC=1;	 WindingD=1;    // Stepper Motor will rotate at 90 degree
	 }
	 if (tenth==6)
	 {
		 WindingA=0;   WindingB=0;	WindingC=1;	 WindingD=0;    // Stepper Motor will rotate at 135 degree
	 }
	 if (tenth==7)
	 {
		 WindingA=0;   WindingB=1;	WindingC=1;	 WindingD=0;    // Stepper Motor will rotate at 180 degree
	 }
	 if (tenth==8)
	 {
		 WindingA=1;   WindingB=1;	WindingC=1;	 WindingD=1;    // Stepper Motor will rotate at 225 degree
	 }
}

void delay (int d)
{
	unsigned char i;
	for(;d>0;d--)
	{
 		for(i=250;i>0;i--);
		for(i=248;i>0;i--);
	}
}
 unsigned char VoltageADC()     // Converts Analog Voltage to Digital Binary Form
{
	unsigned char voltage;        // variable for storing ADC output value
	In_C =0;
	In_B =0;    			// Select Channel 0 for Voltage 
	In_A =0;
		
	ALE = 1;   // Address Latch Enable High means address of channel 0 is selected 
	
  SC=1;      // SC means Start Conversion , L to H Pulse, Conversion Starts

  ALE = 0;   // Again '0' for next address as in Timing diagram
  SC = 0;	   // Again '0' for next Conversion as in Timing diagram
	
	while(EOC==1);	// First, wait till INTR gets '0' by Hardware 
	while(EOC==0);  // Then, also wait for '1' which means conversion done successfully 
	  
	OE=1;				// OE or RD now, we can Latch dataa by H to L Pulse
	  
	voltage=output;  // Store the ADC output dataa in variable voltage
	  
  OE = 0;   // Clear or forced to zero for the next dataa latch
	return voltage;  // Function return the value of voltage 
}
void VoltageConvert(unsigned char voltage)  // Return value of Voltage enter as an argument to convert into voltage 
{
  unsigned char	voltagevalue;
  
	voltagevalue = (voltage*5.02)/256;    // Convert output value into voltage by formula

	SBUF = voltagevalue+0x30;             // To Display on LCD, Convert into ASCI for this we, OR with 0x30
	while (TI==0);                        // Minotor TI , when raised it means dataa is transmitted succesfully
	TI=0;                                 // Forced to 0 for next dataa transmission to avoid overwriting
}
unsigned char TempADC ()    // Converts Analog Temp to Digital Binary Form
{
	unsigned char	tempvalue;  // Variable for storing ADC output value
	
	In_C =0;
	In_B =0;   // Selecting Channel 1  for Temp
	In_A =1;
		
	ALE = 1;   // Address Latch Enable High means address of channel 1 is selected 
	
  SC=1;			// SC means Start Conversion , L to H Pulse, Conversion Starts
 	
  ALE = 0;  // Again '0' for next address as in Timing diagram
  SC = 0;	  // Again '0' for next Conversion as in Timing diagram
	
	while(EOC==1);				// First, wait till INTR gets '0' by Hardware 
	while(EOC==0);  			// Then, also wait for '1' which means conversion done successfully 
	
	OE=1;									// OE or RD now, we can Latch dataa by H to L Pulse
	delay(50);  
	
	tempvalue=output;    // Store the ADC output dataa in variable tempvalue
	  
  OE = 0;   					// Clear or forced to zero for the next dataa latch
  delay(60);
	return tempvalue;   // Function return the value of temperature
}
void TempConvert(unsigned char temp)  // Return value of Temp enter as an argument to convert into Temperature 
{
	unsigned char unit,tenth;    // Unit for LSB, tenth for MSB
  
	temp = temp*1.966;    // Convert temp value of ADC into Temperature by Formula 
			
	tenth = temp/10;      // Divide by 10 to get the ten value
	tenth = tenth+0x30;   // To Display on LCD, Convert into ASCI for this we, OR with 0x30
	SBUF = tenth;         // Stores ASCII of ten value in SBUF and transmit       
	while (TI==0);        // Minotor TI , when raised it means dataa is transmitted succesfully
	TI=0;                 // Forced to 0 for next dataa transmission to avoid overwriting
	
	// Practically delay of 71 ms calling 5 times
	Timerdelay();Timerdelay();Timerdelay();Timerdelay();Timerdelay();
 
	unit = temp%10;       // For unit value, we find remainder value we use '%'
	unit = unit+0x30;     // To Display on LCD, Convert into ASCI for this we, OR with 0x30
	
	SBUF = unit;          // Stores ASCII of unit value in SBUF and transmit
	while (TI==0);				// Minotor TI , when raised it means dataa is transmitted succesfully
	TI=0;                 // Forced to 0 for next dataa transmission to avoid overwriting
}
 unsigned char PressureADC ()     // Converts Analog Pressure to Digital Binary Form
{
	unsigned char pressure;
	In_C =0;
	In_B =1; 		 // Selecting Channel 2 for Pressure
	In_A =0;
		
	ALE = 1;     // Address Latch Enable High means address of channel 2 is selected
	
	SC=1;			   // SC means Start Conversion , L to H Pulse, Conversion Starts
  ALE = 0;		 // Again '0' for next address as in Timing diagram
  SC = 0;	 		 // Again '0' for next Conversion as in Timing diagram
	
	while(EOC==1);	// First, wait till INTR gets '0' by Hardware 
	while(EOC==0);  // Then, also wait for '1' which means conversion done successfully 
	  
	OE=1;						// OE or RD now, we can Latch dataa by H to L Pulse
	delay(50);
	
  // P1=output, after conversion, hex or dec value is stored in Output that is P1. Now, we can read it
	pressure = output;  // Store the ADC output dataa in variable pressure
	  
  OE = 0;   				// Clear or forced to zero for the next dataa latch
  delay(60);
	return pressure;  // Function return the value of pressure
}
void PressureConvert(unsigned char pressure)   // Return value of pressure enter as an argument to convert into Pressure 
{
	unsigned char unit,tenth;
	float vout;
  unsigned char	pressurevalue; 
		
	vout = (5.0/256)* pressure;   				        // 256 due to 8 bits converter, gets the output voltage
	pressurevalue = ((vout/5.0)+0.09)/0.0089-1;   // Convert Output Voltage to Pressure (KPA)
			
	tenth = pressurevalue/10;		// Divide by 10 to get the ten value
	tenth = tenth+0x30;					// To Display on LCD, Convert into ASCI for this we, OR with 0x30
	
	SBUF = tenth;								// Stores ASCII of ten value in SBUF and transmit       
	while (TI==0);							// Minotor TI , when raised it means dataa is transmitted succesfully
	TI=0;												// Forced to 0 for next dataa transmission to avoid overwriting
	
	// Practically delay of 71 ms calling 5 times
	Timerdelay();Timerdelay();Timerdelay();Timerdelay();Timerdelay();
	
	unit = pressurevalue%10;		// For unit value, we find remainder value we use '%'
	unit = unit+0x30;						// To Display on LCD, Convert into ASCI for this we, OR with 0x30
	
	SBUF = unit;								// Stores ASCII of unit value in SBUF and transmit
	while (TI==0);							// Minotor TI , when raised it means dataa is transmitted succesfully
	TI=0;	  										// Forced to 0 for next dataa transmission to avoid overwriting
}
