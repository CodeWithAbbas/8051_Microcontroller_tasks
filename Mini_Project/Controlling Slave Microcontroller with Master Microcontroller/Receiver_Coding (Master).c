#include <reg51.h>
#include <stdio.h>
#define LCD P1  

// Declaring Data Bus and Control Bus 
sbit RS = P3^2;
sbit E = P3^3;

// Declaring Row of Keypad 
sbit R1 = P2^0;
sbit R2 = P2^1;
sbit R3 = P2^2;
sbit R4 = P2^3;

// Declaring Column of Keypad
sbit C1 = P2^4;
sbit C2 = P2^5;
sbit C3 = P2^6;

// User Defined Function Prototyping
void lcdinitial (void);
void lcdcmd (unsigned char);
void lcd_data (unsigned char);
void string (unsigned char *);
void delay (unsigned int);
void keypad(void);

// For receiving Sensing Data user-defined function 
void VoltSensor ();
void TempSensor ();
void PressureSensor ();

// Main Function
void main(void)
 { 
	 LCD = 0x00;  		// Declaring P3 as Output Port
	 lcdinitial();		// Initialize the LCD 
	 TMOD = 0x20;     // Timer 1, Mode 2 which is 8 bits Autoreload
	 TH1 = 0xFD;			// Baud rate is 9600, same as Slave microcontroller to communicate
	 SCON = 0x50;     // 01010000 means SMO=0, SM1=1 which is Mode 2, 8 bits with start and stop bits 
	 TR1 = 1;					// Start Timer 1 for Baud rate
	
	 string ("PRESS * FOR");
   lcdcmd (0xc0); // Send Command to LCD , Move to Next Line	
   string ("THE MENU BAR");
   lcdcmd(0x80); // Beginning of 1st line	 
	 while (1)
	 { 
	  keypad();	 		// Calling keypad function
	 }	 
	//  END OF MAIN FUNCTION
}
void keypad()
 {
	 // Button is Pressed = 0, Button is Not Pressed = 1 
	 R1=0;   						// Row 1 is Pressed 
	 if (C1==0) 				// Column 1 is pressed for Voltage Data
	 { 	
		 lcdcmd(0x01); 		// Clear Screen
		 SBUF = 'V';	 		// Store 'V' character in SBUF Register and transmit
		 while (TI==0); 	// Minotor TI , when raised it means dataa is transmitted succesfully
		 TI=0;						// Forced to 0 for next Data transmission to avoid overwriting
		 
		 string ("VOLTAGE :");
		 lcdcmd (0xc0); 		// Send Command to LCD , Move to Next Line
		 VoltSensor (); 		// Calling function to receive Voltage Sensing values
		 string (" V");	
		 lcdcmd (0x80);			// Beginning of 1st line
	 } 
	  if (C2==0)  				// In keypad, R1 and C2 contain '2' for Temperature Data
	 {
		  lcdcmd(0x01); 		// Clear Screen
		  SBUF = 'T';				// Store 'T' character in SBUF Register and transmit
		  while (TI==0);		// Minotor TI , when raised it means dataa is transmitted succesfully
		  TI=0;							// Forced to 0 for next Data transmission to avoid overwriting
		 
		  string ("TEMPERATURE :");
		  lcdcmd (0xc0); 		// Send Command to LCD , Move to Next Line
		  TempSensor();  		// Calling function to receive Temperature Sensing values
		  string (" C");
		  lcdcmd (0x80);
	 } 
	  if (C3==0)				// In keypad, R1 and C3 contain '3' for Pressure Data
	 {
		  lcdcmd(0x01); 	// Clear Screen
		  SBUF = 'P';			// Store 'P' character in SBUF Register and transmit
		  while (TI==0);	// Minotor TI , when raised it means dataa is transmitted succesfully
		  TI=0;						// Forced to 0 for next Data transmission to avoid overwriting
		  
		  string ("PRESSURE :");
		  lcdcmd (0xc0); 				// Send Command to LCD , Move to Next Line
		  PressureSensor ();		// Calling function to receive Pressure Sensing values
		  string (" KPA");	
		  lcdcmd (0x80);
	 } 
	 R2 = 0; R1=1;  // Row 2 is pressed and Row 1 is not pressed
	 if (C1==0) 
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	  if (C2==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	  if (C3==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	 R3=0; R2=1;  // Row 3 is pressed , Row 2 is un pressed
	 if (C1==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	  if (C2==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	  if (C3==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	 R4=0; R3=1;  // Row 4 is pressed , Row 3 is un pressed
	 if (C1==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("1.VOLTAGE 2.TEMP");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("3.PRESSURE");	
		 lcdcmd (0x80);
	 } 
	  if (C2==0)
	 {
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	  if (C3==0)
	 { 
		 lcdcmd(0x01); // Clear Screen
		 string ("Kindly Press the");
		 lcdcmd (0xc0); // Send Command to LCD , Move to Next Line
		 string ("Valid Key..");	
		 lcdcmd (0x80);
	 } 
	R4=1;   // Row 4 is not pressed 
 }
 // Receiving the voltage Data from Slave Microcontroller
  void VoltSensor ()
 {
	 unsigned char Rx;		// Variable Rx for storing received Data
	 while (RI ==0);      // Minotor RI , when raised it means dataa is received succesfully
	 RI=0;								// Forced to 0 for next Data receiving to avoid overwriting
	 Rx = SBUF;						// Stores the value of SBUF in Rx to secure our Data from lost
	 lcd_data(Rx);				// Display the Voltage received value on the LCD Screen
 }
 // Receiving the Temperature Data from Slave Microcontroller 
 void TempSensor ()
 {
	 unsigned char MSB,LSB;		//MSB means Ten value , LSB means Unit Value
	 while (RI ==0);					// Minotor RI , when raised it means dataa is received succesfully
	 RI=0;										// Forced to 0 for next Data receiving to avoid overwriting
	
	 MSB = SBUF;			 				// Stores the value of SBUF in MSB to secure our Data from lost
	 lcd_data(MSB);		 				// Display the Received Temp Ten value on the LCD Screen
	 
	 while (RI ==0);				// Minotor RI , when raised it means dataa is received succesfully
	 RI=0;									// Forced to 0 for next Data receiving to avoid overwriting
	 LSB = SBUF;						// Stores the value of SBUF in LSB to secure our Data from lost
	 lcd_data(LSB);   			// Display the Received Temp Unit value on the LCD Screen
 }
 // Receiving the Pressure Data from Slave Microcontroller
 void PressureSensor ()
 {
	 unsigned char MSB,LSB;   //MSB means Upper ten value , LSB means unit value
	 while (RI ==0);					// Minotor RI , when raised it means dataa is received succesfully
	 RI=0;										// Forced to 0 for next Data receiving to avoid overwriting
	 MSB = SBUF;							// Stores the value of SBUF in MSB to secure our Data from lost
	 lcd_data(MSB);						// Display the Received Pressure Ten value on the LCD Screen
	
	 while (RI ==0);					// Minotor RI , when raised it means dataa is received succesfully
	 RI=0;										// Forced to 0 for next Data receiving to avoid overwriting
	 LSB = SBUF;							// Stores the value of SBUF in LSB to secure our Data from lost
	 lcd_data(LSB);						// Display the Received Pressure Unit value on the LCD Screen
 }
void lcdinitial (void)				// Initialization of LCD
 {
	lcdcmd(0x0c); // Display On , Cursor Off
	lcdcmd(0x38); // Use two Lines
	lcdcmd(0x01); // Clear Screen
	lcdcmd(0x80); // Beginning of 1st line
 }
 void lcdcmd (unsigned char A)      // Read commands
 {
	 LCD = A;
	 RS = 0;
	 E=1;
	 delay (10);
	 E=0;
 }
 void lcd_data (unsigned char B)	// Write Commands
 {
	 LCD = B;
	 RS = 1;
	 E=1;
	 delay (10);
	 E=0;
 }
 void string (unsigned char *C)
 {
	 while(*C)
	 {
		 lcd_data(*C++);
	 }
 }
 void delay (unsigned int t)
 {
	 int i,j;
	
	 for (i=1; i<=t; i++)
	 for (j=1; j<=1000; j++); 
 }
  
 