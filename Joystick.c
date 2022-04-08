/*
1 
Nintendo Switch Fightstick - Proof-of-Concept

Based on the LUFA library's Low-Level Joystick Demo
	(C) Dean Camera
Based on the HORI's Pokken Tournament Pro Pad design
	(C) HORI

This project implements a modified version of HORI's Pokken Tournament Pro Pad
USB descriptors to allow for the creation of custom controllers for the
Nintendo Switch. This also works to a limited degree on the PS3.

Since System Update v3.0.0, the Nintendo Switch recognizes the Pokken
Tournament Pro Pad as a Pro Controller. Physical design limitations prevent
the Pokken Controller from functioning at the same level as the Pro
Controller. However, by default most of the descriptors are there, with the
exception of Home and Capture. Descriptor modification allows us to unlock
these buttons for our use.
*/

#include "Joystick.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include "uart.h"

#define BAUD_RATE 9600

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

// write a string to the uart
#define uart_print(s) uart_print_P(PSTR(s))
void uart_print_P(const char *str)
{
	char c;
	while (1) {
		c = pgm_read_byte(str++);
		if (!c) break;
		uart_putchar(c);
	}
}

typedef enum {
	L_UP,
	L_DOWN,
	L_LEFT,
	L_RIGHT,
	R_UP,
	R_DOWN,
	R_LEFT,
	R_RIGHT,
	X,
	Y,
	A,
	B,
	L,
	R,
	THROW,
	NOTHING,
	TRIGGERS,
	HOME,
	MINUS
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command; 

static command step[] = {
	// Setup controller
	//{ NOTHING,  250 },
	//{ TRIGGERS,   5 },
	//{ NOTHING,  150 },
	//{ TRIGGERS,   5 },
	//{ NOTHING,  150 },
	//{ A,          5 },
	//{ NOTHING,  250 },
	//{ NOTHING,   500 }, //RN: 40 = ~1 second on Switch
{NOTHING,     100},
{L_DOWN,        3},
{NOTHING,       1},
{L_RIGHT,       3},
{NOTHING,       1},
{L_RIGHT,       3},
{NOTHING,       1},
{L_RIGHT,       3},
{NOTHING,       1},
{L_RIGHT,       3},
{NOTHING,       1},
{A,             5},
{NOTHING,      55},
{A,             5},
{NOTHING,      35},
{A,             5},
{NOTHING,     120},
{A,             5},
{NOTHING,      80},
{B,             5},
{NOTHING,      55},
{L_UP,          3},
{NOTHING,       5},
{L_LEFT,        3},
{NOTHING,       5},
{L_LEFT,        3},
{NOTHING,       5},
{A,             5},
{NOTHING,     100}


//{L_DOWN,        6},
//{L_LEFT,       76},
//{L_UP,         15},
//{A,             5}, //Enter the shop
//{NOTHING,     350}, //Shop loading
//{L_UP,         30},
//{NOTHING,      40},
//{L_LEFT,       15},
//{NOTHING,      20},
//{A,             5},
//{NOTHING,      80},
//{A,             5},
//{NOTHING,      40},
//{A,             5},
//{NOTHING,      20},
//{L_DOWN,        5},
//{NOTHING,       5},
//{A,             3}, //Turnip Price Today?
//{R,           360}, //Pause for a while to let you see the price
//{B,             5},
//{NOTHING,      20},
//{A,             5},
//{NOTHING,      10},
//{B,             5},
//{NOTHING,      10},
//{B,             5},
//{NOTHING,      20},
//{B,             5},
//{NOTHING,      20},
//{B,             5},
//{NOTHING,      20},
//{B,             5},
//{NOTHING,      20},
//{B,             5},
//{NOTHING,      20},
//{B,             5},
//{NOTHING,      40}, //Before Minus
//{MINUS,         5},
//{NOTHING,      80},
//{A,             5},
//{NOTHING,      20},
//{A,             5}, //Save and exit
//{NOTHING,     440}, //Saving…Don't turn off the power…
//{HOME,          5}, //Back to the OS
//{NOTHING,      55},
//{L_DOWN,        3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{A,             5}, //Click on Settings Icon
//{NOTHING,      55},
//{L_DOWN,       60},
//{NOTHING,      10},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_DOWN,        3},
//{NOTHING,       1},
//{L_DOWN,        3},
//{NOTHING,       1},
//{L_DOWN,        3},
//{NOTHING,       1},
//{L_DOWN,        3},
//{NOTHING,       1},
//{A,             5}, //Click on Date and Time
//{NOTHING,      10},
//{L_DOWN,        3},
//{NOTHING,       1},
//{L_DOWN,        3},
//{NOTHING,       1},
//{A,             5}, //Click on Current Date and Time
//{NOTHING,      30},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{L_RIGHT,       3},
//{NOTHING,       1}, //Cursor on Minute
//{L_DOWN,        5},
//{NOTHING,      10},
//{L_RIGHT,       3},
//{NOTHING,       1},
//{A,             5}, //Click on OK
//{NOTHING,      20},
//{HOME,          5},
//{NOTHING,      40}, //Back to the OS
//{HOME,          5},
//{NOTHING,      60}, //Back to the game
//{A,             5},
//{NOTHING,     700}, //Wait for title screen to appear
//{A,             5}, //Spam on A…
//{NOTHING,       5},
//{A,             5},
//{NOTHING,       5},
//{A,             5},
//{NOTHING,       5},
//{A,             5},
//{NOTHING,       5},
//{A,             5},
//{NOTHING,       5},
//{A,             5},
//{NOTHING,       5},
//{A,             5},
//{NOTHING,     800}, //Wait until the character appear in front of the house
//{NOTHING,     100}

};

// Main entry point.
int main(void) {
	CPU_PRESCALE(0);
	uart_init(BAUD_RATE);
	uart_print("UART Example\r\n");
	
	// We'll start by performing hardware and peripheral setup.
	SetupHardware();
	// We'll then enable global interrupts for our use.
	GlobalInterruptEnable();
	// Once that's done, we'll enter an infinite loop.
	for (;;)
	{
		// We need to run our task to process and deliver data for our IN and OUT endpoints.
		HID_Task();
		// We also need to run the main USB management task.
		USB_USBTask();
	}
}

// Configures hardware and peripherals, such as the USB peripherals.
void SetupHardware(void) {
	// We need to disable watchdog if enabled by bootloader/fuses.
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// We need to disable clock division before initializing the USB hardware.
	clock_prescale_set(clock_div_1);
	// We can then initialize our hardware and peripherals, including the USB stack.

	#ifdef ALERT_WHEN_DONE
	// Both PORTD and PORTB will be used for the optional LED flashing and buzzer.
	#warning LED and Buzzer functionality enabled. All pins on both PORTB and \
PORTD will toggle when printing is done.
	//DDRD  = 0xFF; //Teensy uses PORTD
	DDRD  = 0xFE; //RN: Set D0 to input(0)
	PORTD =  0x01;
                  //We'll just flash all pins on both ports since the UNO R3
	DDRB  = 0xFF; //uses PORTB. Micro can use either or, but both give us 2 LEDs
	PORTB =  0x0; //The ATmega328P on the UNO will be resetting, so unplug it?
	#endif
	// The USB stack should be initialized last.
	USB_Init();
}

// Fired to indicate that the device is enumerating.
void EVENT_USB_Device_Connect(void) {
	// We can indicate that we're enumerating here (via status LEDs, sound, etc.).
}

// Fired to indicate that the device is no longer connected to a host.
void EVENT_USB_Device_Disconnect(void) {
	// We can indicate that our device is not ready (via status LEDs, sound, etc.).
}

// Fired when the host set the current configuration of the USB device after enumeration.
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	// We setup the HID report endpoints.
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_OUT_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(JOYSTICK_IN_EPADDR, EP_TYPE_INTERRUPT, JOYSTICK_EPSIZE, 1);

	// We can read ConfigSuccess to indicate a success or failure at this point.
}

// Process control requests sent to the device from the USB host.
void EVENT_USB_Device_ControlRequest(void) {
	// We can handle two control requests: a GetReport and a SetReport.

	// Not used here, it looks like we don't receive control request from the Switch.
}

// Process and deliver data from IN and OUT endpoints.
void HID_Task(void) {
	// If the device isn't connected and properly configured, we can't do anything here.
	if (USB_DeviceState != DEVICE_STATE_Configured)
		return;

	// We'll start with the OUT endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_OUT_EPADDR);
	// We'll check to see if we received something on the OUT endpoint.
	if (Endpoint_IsOUTReceived())
	{
		// If we did, and the packet has data, we'll react to it.
		if (Endpoint_IsReadWriteAllowed())
		{
			// We'll create a place to store our data received from the host.
			USB_JoystickReport_Output_t JoystickOutputData;
			// We'll then take in that data, setting it up in our storage.
			while(Endpoint_Read_Stream_LE(&JoystickOutputData, sizeof(JoystickOutputData), NULL) != ENDPOINT_RWSTREAM_NoError);
			// At this point, we can react to this data.

			// However, since we're not doing anything with this data, we abandon it.
		}
		// Regardless of whether we reacted to the data, we acknowledge an OUT packet on this endpoint.
		Endpoint_ClearOUT();
	}

	// We'll then move on to the IN endpoint.
	Endpoint_SelectEndpoint(JOYSTICK_IN_EPADDR);
	// We first check to see if the host is ready to accept data.
	if (Endpoint_IsINReady())
	{
		// We'll create an empty report.
		USB_JoystickReport_Input_t JoystickInputData;
		// We'll then populate this report with what we want to send to the host.
		GetNextReport(&JoystickInputData);
		// Once populated, we can output this data to the host. We do this by first writing the data to the control stream.
		while(Endpoint_Write_Stream_LE(&JoystickInputData, sizeof(JoystickInputData), NULL) != ENDPOINT_RWSTREAM_NoError);
		// We then send an IN packet on this endpoint.
		Endpoint_ClearIN();
	}
}

typedef enum {
	SYNC_CONTROLLER,
	SYNC_POSITION,
	BREATHE,
	PROCESS,
	CLEANUP,
	DONE
} State_t;
State_t state = SYNC_CONTROLLER;

#define ECHOES 2
int echoes = 0;
USB_JoystickReport_Input_t last_report;

int report_count = 0;
int xpos = 0;
int ypos = 0;
int bufindex = 0;
int cleanup_index = 28;
int duration_count = 0;
int portsval = 0;
int count_iter = 0;

uint8_t c;
char arrCOMInput[64] = {};

// ProcessButtonEvent sets the corresponding button status in ReportData
void ProcessButtonEvent(USB_JoystickReport_Input_t* const ReportData)
{
	switch (step[bufindex].button)
	{

		case L_UP:
			ReportData->LY = STICK_MIN;				
			break;

		case L_LEFT:
			ReportData->LX = STICK_MIN;				
			break;

		case L_DOWN:
			ReportData->LY = STICK_MAX;				
			break;

		case L_RIGHT:
			ReportData->LX = STICK_MAX;				
			break;
			
		case R_UP:
			ReportData->RY = STICK_MIN;				
			break;

		case R_LEFT:
			ReportData->RX = STICK_MIN;				
			break;

		case R_DOWN:
			ReportData->RY = STICK_MAX;				
			break;

		case R_RIGHT:
			ReportData->RX = STICK_MAX;				
			break;
		case A:
			ReportData->Button |= SWITCH_A;
			break;

		case B:
			ReportData->Button |= SWITCH_B;
			break;
		case X:
			ReportData->Button |= SWITCH_X;
			break;

		case Y:
			ReportData->Button |= SWITCH_Y;
			break;

		case R:
			ReportData->Button |= SWITCH_R;
			break;

		case THROW:
			ReportData->LY = STICK_MIN;				
			ReportData->Button |= SWITCH_R;
			break;

		case TRIGGERS:
			ReportData->Button |= SWITCH_L | SWITCH_R;
			break;

		case HOME:
			ReportData->Button |= SWITCH_HOME;
			break;
			
		case MINUS:
			ReportData->Button |= SWITCH_MINUS;
			break;

		default:
			ReportData->LX = STICK_CENTER;
			ReportData->LY = STICK_CENTER;
			ReportData->RX = STICK_CENTER;
			ReportData->RY = STICK_CENTER;
			ReportData->HAT = HAT_CENTER;
			break;
	}
}

// Prepare the next report for the host.
void GetNextReport(USB_JoystickReport_Input_t* const ReportData) {

	// Prepare an empty report
	memset(ReportData, 0, sizeof(USB_JoystickReport_Input_t));
	ReportData->LX = STICK_CENTER;
	ReportData->LY = STICK_CENTER;
	ReportData->RX = STICK_CENTER;
	ReportData->RY = STICK_CENTER;
	ReportData->HAT = HAT_CENTER;

	// Repeat ECHOES times the last report
	if (echoes > 0)
	{
		memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
		echoes--;
		return;
	}

	// States and moves management
	switch (state)
	{

		case SYNC_CONTROLLER:
			state = BREATHE;
			break;

		// case SYNC_CONTROLLER:
		// 	if (report_count > 550)
		// 	{
		// 		report_count = 0;
		// 		state = SYNC_POSITION;
		// 	}
		// 	else if (report_count == 250 || report_count == 300 || report_count == 325)
		// 	{
		// 		ReportData->Button |= SWITCH_L | SWITCH_R;
		// 	}
		// 	else if (report_count == 350 || report_count == 375 || report_count == 400)
		// 	{
		// 		ReportData->Button |= SWITCH_A;
		// 	}
		// 	else
		// 	{
		// 		ReportData->Button = 0;
		// 		ReportData->LX = STICK_CENTER;
		// 		ReportData->LY = STICK_CENTER;
		// 		ReportData->RX = STICK_CENTER;
		// 		ReportData->RY = STICK_CENTER;
		// 		ReportData->HAT = HAT_CENTER;
		// 	}
		// 	report_count++;
		// 	break;

		case SYNC_POSITION:
			bufindex = 0;


			ReportData->Button = 0;
			ReportData->LX = STICK_CENTER;
			ReportData->LY = STICK_CENTER;
			ReportData->RX = STICK_CENTER;
			ReportData->RY = STICK_CENTER;
			ReportData->HAT = HAT_CENTER;


			state = BREATHE;
			break;

		case BREATHE:
			state = PROCESS;
			break;

		case PROCESS:
			ProcessButtonEvent(ReportData);

			duration_count++;

			if (duration_count > step[bufindex].duration)
			{
				bufindex++;
				duration_count = 0;				
			}
			
			if (bufindex == cleanup_index) //reached the bottom of main action loop
			{
				bufindex = 28; //27+25
				duration_count = 0;

				state = BREATHE;

				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
				
				state = DONE;
				
			}

			if (bufindex > (int)( sizeof(step) / sizeof(step[0])) - 1)
			{

				state = DONE; //Should not happen, unless there are no clean up activities defined
//				state = BREATHE;

			}

			break;

		case CLEANUP:
			ProcessButtonEvent(ReportData);
			
			duration_count++;

			if (duration_count > step[bufindex].duration)
			{
				bufindex++;
				duration_count = 0;				
			}

			if (bufindex > (int)( sizeof(step) / sizeof(step[0])) - 1)
			{
				ReportData->LX = STICK_CENTER;
				ReportData->LY = STICK_CENTER;
				ReportData->RX = STICK_CENTER;
				ReportData->RY = STICK_CENTER;
				ReportData->HAT = HAT_CENTER;
				state = DONE;
			}
			
			break;

		case DONE:
			#ifdef ALERT_WHEN_DONE
			//portsval = ~portsval;
			if (!(PIND & 0x01))
			{
				PORTD |= 0x40;
				_delay_ms(3000);
				state = SYNC_POSITION;
				PORTD ^= 0x40;
				count_iter = 0;
				return;
			}
			
			//PORTD ^= 0x40;
			//PORTD = portsval; //flash LED(s) and sound buzzer if attached
			//PORTB = portsval;
			//_delay_ms(250);
			#endif
			// ^= XOR: 1 if bit values are different. 
			// 0x40 = (1<<6) = 0b01000000 = 6th bit 
			// (starting from the least significant bit (LSB) = 0th bit, 
			// AKA right-most bit).
			// Purpose here is to flip the on-board LED on PD6 (refer to Pinout card)
			// The pins are named with a letter and number. Pins are grouped into 
			// 8 bit ports represented by the letter, and the number represents the 
			// individual bit within the port. B4 is bit 4 in port B, for example.
			PORTD ^= 0x40;
			if (duration_count == 0){
				while (1) {
					//uart_print("LOOP");
					
					// Syntax of input from serial port:
					// ![button][,button...]@[duration]#
					if (uart_available()) {
						PORTD ^= 0x40;
						c = uart_getchar();
						uart_putchar(c);
						// '!' is the beginning character of an event
						if (c == '!') {
							memset(arrCOMInput,0,64*sizeof(char));
							int iCount = 0;
							while (1)
							{
								c = uart_getchar();
								uart_putchar(c);
								// '@' delimits button number and event duration
								if (c == '@') break;
								// ',' is used to separate the button numbers in case
								// multiple buttons need to be pressed at the same time
								if (c == ',')
								{
									arrCOMInput[iCount] = '\0';
									step[bufindex].button = atoi(arrCOMInput);
									memset(arrCOMInput,0,64*sizeof(char));
									iCount = 0;
									ProcessButtonEvent(ReportData);
									continue;
								}
								arrCOMInput[iCount] = c;
								iCount++;
							}
							arrCOMInput[iCount] = '\0';
							step[bufindex].button = atoi(arrCOMInput);
							
							memset(arrCOMInput,0,64*sizeof(char));
							iCount = 0;
							while (1)
							{
								c = uart_getchar();
								uart_putchar(c);
								if (c == '#') break;
								arrCOMInput[iCount] = c;
								iCount++;
							}							
							arrCOMInput[iCount] = '\0';
							step[bufindex].duration = atoi(arrCOMInput);
							
							ProcessButtonEvent(ReportData);
							uart_print("$");
							duration_count++;
							break;
						}
						else if (c == '^')
						{
							state = SYNC_POSITION;
							return;
						}
					}
				}
			}
			else {
				duration_count++;
				if (duration_count > step[bufindex].duration) {
					// Release all buttons and joystick,
					// and reset the duration count so it goes back
					// to the loop of receiving serial port input
					step[bufindex].button = 15; //15 = NOTHING
					duration_count = 0;
					ProcessButtonEvent(ReportData);					
				}
				else
				{
					// Duration not reached. Carry on with the last event
					memcpy(ReportData, &last_report, sizeof(USB_JoystickReport_Input_t));
				}
				
				uart_print("%");
			}

			
		

	}

	// // Inking
	// if (state != SYNC_CONTROLLER && state != SYNC_POSITION)
	// 	if (pgm_read_byte(&(image_data[(xpos / 8) + (ypos * 40)])) & 1 << (xpos % 8))
	// 		ReportData->Button |= SWITCH_A;

	// Prepare to echo this report
	memcpy(&last_report, ReportData, sizeof(USB_JoystickReport_Input_t));
	echoes = ECHOES;

}
