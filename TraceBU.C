
//AVR Microcontroller libraries. Like stdio.h or conio.h but specific to the'a0
//type of microcontroller inside the iRobot
#include <avr/io.h>
#include <avr/interrupt.h>

/*********************
* Function Prototypes
/********************/
void setupSerial(void);
void setupTimer(void);
void byteTx(uint8_t);
void turn90(void);
void turn180(void);
void delayMs(uint16_t);
void clearReceiveBuffer(void);
uint8_t byteRx(void);

/**********************
* Variable Declarations
/*********************/
volatile uint16_t timerCount = 0;
volatile uint8_t timerRunning = 0;


int main(void){
	/**************************
	* Run intialization methods.
	/**************************/
	cli();
	//Properly set serial pins.
	setupSerial();
	//Create the 1 microsecond interrupt.
	setupTimer();
	sei();
	//Your code goes here!
	
	
	byteTx(128); //start robot
	byteTx(132); //full mode
	delayMs(5); //give it some time to do these things
	
	
	forward(2);
	turn(45);
	forward(0);
	turn(45);
	forward(1);
	turn(45);
	forward(0);
	turn(45);
	
	forward(2);
	turn(180);
	
	forward(2);
	turn(45);
	forward(0);
	turn(45);
	forward(1);
	turn(45);
	forward(0);
	turn(45);
	forward(2);
	
	turn(90);
	forward(6);
	
	turn(90);
	forward(4);
	
	turn(90);
	forward(5);
	turn(-45);
	forward(0);
	turn(-45);
	forward(2);
	turn(-45);
	forward(0);
	turn(-45);
	forward(5);
	

}
//Any methods you want to write can go here!


void forward(int units)
{
	byteTx(145);
	byteTx(0);
	byteTx(200);
	byteTx(0);
	byteTx(201); //Because the right wheel is a bit too strong, left wheel faster
	setupTimer();
	
	//Special Case (for when we need to go root 2 units)
	if(units==0)
	{
		delayMs(2156);
	}
		
	//In all other cases... go 1 unit until you reach number of units (1 unit = 1 foot)
	for(int toGo=0;toGo<units;toGo++)
	{

		delayMs(1525);
	}
	
	byteTx(145); 'a0		//calls the drive command (145) and sets wheel velocities back to 0'a0
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);'a0
}

void turn(int degreesToTurn)
{
	//Start both wheels to turn at a radius
	byteTx(137);
	byteTx(0);
	byteTx(100);
	
	//Let's set positive to turn clockwise, so negative will be counterclockwise
	if(degreesToTurn<0)
	{
		//If negative, turn counterclockwise
		//Hexadecimal 0001 == 00h and 01h == 0d and 1d
		byteTx(0);
		byteTx(1);
	}
	else
	{
		//Otherwise turn clockwise
		//Hexadecimal FFFF == FFh and FFh == 255d and 255d'a0
		byteTx(255);
		byteTx(255);
	}
	setupTimer();
	
	if(degreesToTurn==45 || degreesToTurn == -45)
	{
		delayMs(1036);
	}
	else if(degreesToTurn==90 || degreesToTurn == -90)
	{
		delayMs(2072);
	}
	else if(degreesToTurn==180 || degreesToTurn == -180)
	{
		delayMs(4144);
	}
	else
	{
		delayMs(1);
	}

	byteTx(145); 'a0	//calls the drive command (145) and sets wheel velocities back to 0'a0
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);
'a0 'a0 byteTx(0);'a0
}



/*****************************
*READ ABOUT THESE FUNCTIONS!!!
/****************************/

//This is a one millisecond interrupt function. That means that
//every 1ms, this method gets called by the program, regardless of what
//else is happening. This makes it ideal for timing functions.
//In fact, it is already configured to work properly as a delay timer.
//The DelayMS function utilizes this function to decrease timerRunning once
//every millisecond. Once timerRunning reaches 0, the delayMS function quits'a0
//and the program resumes.
SIGNAL(SIG_OUTPUT_COMPARE1A) {
	if(timerRunning) {
		if(timerCount != 0) {
			timerCount--;
		} else {
				timerRunning = 0;
			}
	}
}

//This is the function for sending data, one byte at a time,
//to the iRobot. It waits until the transmission lines are clear
//and then transmits the byte, value, to the robot.
//So if you wanted to send the number 128 to the iRobot, you would use
//byteTx(128).
void byteTx(uint8_t value) {
	// Transmit one byte to the robot.
	// Wait for the buffer to be empty.
	while(!(UCSR0A & 0x20)) {
	// Do nothing.
	}
	// Send the byte.
	UDR0 = value;
}

//This clears old data out of the input registers.
//Without calling this method before attempting to read in input the
//iRobot will behave very differently than expected. USE THIS BEFORE REQUESTING
//EVERY BYTE OF DATA!!
void clearReceiveBuffer(){
	uint8_t empty; //Buffer drain.
	while(UCSR0A & 0x80) {/* waits for the buffer to empty */
		empty = UDR0;
	}
	
}
//This is the counterpart to byteTx(uint8_t). Where byteTx
//sends a byte, byteRx receives a byte.
//It is important to realize that byteRx will wait until there
//is a byte to receive before doing anything. As a result,
//if you use byteRx, your program will pause until it is sent
//a byte from the iRobot.
uint8_t byteRx(void){
	while(!(UCSR0A & 0x80)) ;
	/* wait until a byte is received */
	return UDR0;
}

/*****************************************************
*	Initialization Code. (dont worry about this stuff)
/****************************************************/
void setupTimer(void) {
	// Set up the timer 1 interupt to be called every 1ms.
	TCCR1A = 0x00;
	TCCR1B = 0x0C;
	OCR1A = 71;
	TIMSK1 = 0x02;
}
void delayMs(uint16_t timeMs) {
	timerCount = timeMs;
	timerRunning = 1;
	//timerCount gets decremented every 1ms by the'a0
	//interrupt function above. Once the interrupt determines
	//that timerCount has been reduced to 0, it sets timerRunning
	//to 0, which will terminate the following while loop.
	while(timerRunning) {
	// do nothing
	}
}

void setupSerial(void) {
	// Set the transmission speed to 57600 baud, which is what the Create expects,
	// unless we tell it otherwise.
	UBRR0 = 19;
	// Enable both transmit and receive.
	UCSR0B = 0x18;
	// Set 8-bit data.
	UCSR0C = 0x06;
}}
