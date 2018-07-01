/*
 * GccApplication8.c
 *
 * Created: 2/24/2018 3:16:17 PM
 * Author : Amin
 */ 
//Clock Speed
#define F_CPU 8000000
#define INSTR_PER_US 8                  
#define INSTR_PER_MS 8000               
#define MAX_RESP_TIME_MS 100
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

//State Machines
#define TURNLENGTH 3
volatile unsigned char mode = 3;
volatile unsigned char move = 0;
unsigned char rightBlocked = 0;
unsigned char dur = 0;
//Timer Flags
volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;
void TimerOn();
void TimerOff();
void TimerISR();
void TimerSet(unsigned long M);

//Ultra Sonic
volatile uint32_t pulse = 0;
volatile unsigned char UltraFlag = 0;
volatile uint32_t distance = 11;
volatile uint32_t oldDistance = 0;
volatile unsigned char measuring = 0;
volatile uint32_t counter = 0;
void initUltra();
void measureDistance();


//USART 
#define USART_BAUDRATE 38400
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
volatile unsigned char value;
volatile unsigned char input;
void initUSART();
void sendUSART(uint8_t);
void recieveData();

//Motor
unsigned char motorSignal = 0;
void moveForward();
void moveLeft();
void moveRight();
void moveLeft();
void stopMotor();
void moveBackward();

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a%b;
        if(c==0){return b;}
        a = b;
b = c;
    }
    return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
    /*Tasks should have members that include: state, period,
        a measurement of elapsed time, and a function pointer.*/
    signed char state; //Task's current state
    unsigned long int period; //Task period
    unsigned long int elapsedTime; //Time elapsed since last task tick
    int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//--------Shared Variables----------------------------------------------------


//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
enum SM1_States {SM1_HOLD, SM1_WAIT, SM1_FORWARD, SM1_LEFT, SM1_RIGHT};

int SMTick1(int state) {
	switch(state)
	{
		case SM1_HOLD: if (mode == '1') state = SM1_WAIT;
						else state = SM1_HOLD;
						break;
		case SM1_WAIT:	if (mode != '1') state = SM1_HOLD;
		else if (move == '3' && distance > 20) state = SM1_FORWARD;
		else if (move == '3' && distance <= 20 ) state = SM1_RIGHT;
		else state = SM1_WAIT;
		dur = 0;
		break;
		
		case SM1_FORWARD: if (mode != '1') state = SM1_HOLD;
		else if (move == '3' && distance > 20)
		{
			state = SM1_FORWARD;
			moveForward();
			rightBlocked = 0;
		}
		else if ((move == '3') && (distance <= 20) && (rightBlocked != 1)) state = SM1_RIGHT;
		else if ((move == '3') && (distance <= 20) && (rightBlocked == 1)) state = SM1_LEFT;
		else state = SM1_WAIT;
		break;
		
		case SM1_RIGHT:	if (mode != '1') state = SM1_HOLD;
		else if(dur  <= TURNLENGTH) {
			state = SM1_RIGHT;
			++dur;
		}
		else {
			if (distance < 20) rightBlocked = 1;
			state = SM1_FORWARD;
			dur = 0;
		}
		break;
		
		case SM1_LEFT:	if (mode != '1') state = SM1_HOLD;
		else if(dur <= TURNLENGTH) {
			state = SM1_LEFT;
			++dur;
		}
		else {
			state = SM1_FORWARD;
			dur = 0;
		}
		break;
		default:	break;
	}
	switch(state)
	{
		case SM1_WAIT:	stopMotor();
		break;
		case SM1_FORWARD:	moveForward();
		break;
		case SM1_RIGHT: moveRight();
		break;
		case SM1_LEFT:	moveLeft();
		break;
		
		default:		break;
	}
	return state;
}

//Enumeration of states.
enum SM2_States {SM2_HOLD, SM2_WAIT, SM2_FORWARD, SM2_LEFT, SM2_RIGHT, SM2_BACKWARD};

int SMTick2(int state) {
	switch(state)
	{
		case SM2_HOLD: if (mode == '3') state = SM2_WAIT;
						else state = SM2_HOLD;
						break;
		case SM2_WAIT: 	if (mode != '3') state = SM2_HOLD;
		else if (move == '3'&& (distance > 20)) state = SM2_FORWARD;
		else if (move == '2') state = SM2_RIGHT;
		else if (move == '1') state = SM2_LEFT;
		else if (move == '4') state = SM2_BACKWARD;
		else state = SM2_WAIT;
		break;
		
		case SM2_FORWARD: if (mode != '3') state = SM2_HOLD;
		else if (move == '3' && (distance > 20)) state = SM2_FORWARD;
		else if ((move == '3') && (distance <= 20)) state = SM2_WAIT;
		else if (move == '2') state = SM2_RIGHT;
		else if (move == '1') state = SM2_LEFT;
		else if (move == '4') state = SM2_BACKWARD;
		else state = SM2_WAIT;
		break;
		
		case SM2_RIGHT:	if (mode != '3') state = SM2_HOLD; 
		else if (move == '3' &&(distance >20)) state = SM2_FORWARD;
		else if ((move == '3') && (distance <= 20)) state = SM2_WAIT;
		else if (move == '2') state = SM2_RIGHT;
		else if (move == '1') state = SM2_LEFT;
		else if (move == '4') state = SM2_BACKWARD;
		else state = SM2_WAIT;
		break;
		
		case SM2_LEFT: if (mode != '3') state = SM2_HOLD;
		else if (move == '3'&&(distance > 20) ) state = SM2_FORWARD;
		else if ((move == '3') && (distance <= 20)) state = SM2_WAIT;
		else if (move == '2') state = SM2_RIGHT;
		else if (move == '1') state = SM2_LEFT;
		else if (move == '4') state = SM2_BACKWARD;
		else state = SM2_WAIT;
		break;
		case SM2_BACKWARD:	if (mode != '3') state = SM2_HOLD;
		else if (move == '3'&&(distance > 20) ) state = SM2_FORWARD;
		else if ((move == '3') && (distance <= 20)) state = SM2_WAIT;
		else if (move == '2') state = SM2_RIGHT;
		else if (move == '1') state = SM2_LEFT;
		else if (move == '4') state = SM2_BACKWARD;
		else state = SM2_WAIT;
		break;
		
		default:	break;
	}
	switch(state)
	{
		case SM2_WAIT:	stopMotor();
		break;
		case SM2_FORWARD:moveForward();
		break;
		case SM2_RIGHT: moveRight();
		break;
		case SM2_LEFT:	moveLeft();
		break;
		case SM2_BACKWARD: moveBackward();
		break;
		
		default:		break;
	}
	return state;
}


enum SM3_States {SM3_HOLD, SM3_WAIT, SM3_FORWARD, SM3_LEFT, SM3_RIGHT, SM3_BACKWARD};

int SMTick3(int state) {
	switch(state)
	{
		case SM3_HOLD: if (mode == '2') state = SM3_WAIT;
						else state = SM3_HOLD;
						break;
		case SM3_WAIT: if (mode != '2') state = SM3_HOLD;
		else if (move == '3'&& (distance > 20)) state = SM3_FORWARD;
		else if (move == '2') state = SM3_RIGHT;
		else if (move == '1') state = SM3_LEFT;
		else if (move == '4') state = SM3_BACKWARD;
		else state = SM3_WAIT;
		
		dur = 0;
		break;
		
		case SM3_FORWARD: if (mode != '2') state = SM3_HOLD;
		else if (move == '3' && (distance > 20)) state = SM3_FORWARD;
		else if ((move == '3') && (distance <= 20)) state = SM3_WAIT;
		else if (move == '2') state = SM3_RIGHT;
		else if (move == '1') state = SM3_LEFT;
		else if (move == '4') state = SM3_BACKWARD;
		else state = SM3_WAIT;
		break;
		
		case SM3_RIGHT:	if (mode != '2') state = SM3_HOLD;
		else if(dur  <= TURNLENGTH) {
			state = SM3_RIGHT;
			++dur;
		}
		else {
			state = SM3_FORWARD;
			dur = 0;
			move = '3';
		}
		break;
		
		case SM3_LEFT:	if (mode != '2') state = SM3_HOLD;
		else if(dur <= TURNLENGTH) {
			state = SM3_LEFT;
			++dur;
		}
		else {
			state = SM3_FORWARD;
			dur = 0;
			move = '3';
		}
		break;
		case SM3_BACKWARD:	if (mode != '2') state = SM3_HOLD; 
		else if(dur <= TURNLENGTH) {
			state = SM3_BACKWARD;
			++dur;
		}
		else {
			state = SM3_WAIT;
			move = '0';
			dur = 0;
		}
		break;
		
		default:	break;
	}
	switch(state)
	{
		case SM3_WAIT:	stopMotor();
		break;
		case SM3_FORWARD:	moveForward();
		break;
		case SM3_RIGHT: moveRight();
		break;
		case SM3_LEFT:	moveLeft();
		break;
		case SM3_BACKWARD: moveBackward();
		break;
		
		default:		break;
	}
	return state;
}

//Enumeration of states.
enum SM4_States { SM4_measure_and_send };

// Combine blinking LED outputs from SM2 and SM3, and output on PORTB
int SMTick4(int state) {
    // Local Variables


    //State machine transitions
    switch (state) {
    case SM4_measure_and_send:    break;

    default:        state = SM4_measure_and_send;
                break;
    }

    //State machine actions
    switch(state) {
    case SM4_measure_and_send:   measureDistance();
					value = (uint8_t) distance;
					sendUSART(value);
					break;

    default:        break;
    }

    PORTB = motorSignal; 

    return state;
}

// --------END User defined FSMs-----------------------------------------------

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRB = 0xFF; PORTB = 0;
DDRC = 0xFF; PORTC = 0;
DDRA = 0xFF; PORTA = 0;

// Initialize systems
initUltra();
initUSART();

// Period for the tasks
unsigned long int SMTick1_calc = 200;
unsigned long int SMTick2_calc = 200;
unsigned long int SMTick3_calc = 200;
unsigned long int SMTick4_calc = 300;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
tmpGCD = findGCD(tmpGCD, SMTick3_calc);
tmpGCD = findGCD(tmpGCD, SMTick4_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTick2_period = SMTick2_calc/GCD;
unsigned long int SMTick3_period = SMTick3_calc/GCD;
unsigned long int SMTick4_period = SMTick4_calc/GCD;

//Declare an array of tasks 
static task task1, task2, task3, task4;
task *tasks[] = { &task1, &task2, &task3, &task4 };
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMTick1;//Function pointer for the tick.

// Task 2
task2.state = -1;//Task initial state.
task2.period = SMTick2_period;//Task Period.
task2.elapsedTime = SMTick2_period;//Task current elapsed time.
task2.TickFct = &SMTick2;//Function pointer for the tick.

// Task 3
task3.state = -1;//Task initial state.
task3.period = SMTick3_period;//Task Period.
task3.elapsedTime = SMTick3_period; // Task current elasped time.
task3.TickFct = &SMTick3; // Function pointer for the tick.

// Task 4
task4.state = -1;//Task initial state.
task4.period = SMTick4_period;//Task Period.
task4.elapsedTime = SMTick4_period; // Task current elasped time.
task4.TickFct = &SMTick4; // Function pointer for the tick.

// Set the timer and turn it on
TimerSet(GCD);
TimerOn();

unsigned short i; // Scheduler for-loop iterator
while(1) {
    // Scheduler code
    for ( i = 0; i < numTasks; i++ ) {
        // Task is ready to tick
        if ( tasks[i]->elapsedTime == tasks[i]->period ) {
            // Setting next state for task
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            // Reset the elapsed time for next tick.
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += 1;
    }
    while(!TimerFlag);
    TimerFlag = 0;
	
}

// Error: Program should not exit!
return 0;
}

void measureDistance()
{
	if(!measuring) {
		measuring = 1;
		PORTC |= 0x60;
		
		_delay_us(10);
		PORTC =PORTC & ~(0x60);
	}
}

void moveForward() {
	//Sets enable on both motors, and high on pins 1A, and 3A.
	motorSignal = 0x33;
}

void moveLeft() {
	//Sets enable on both motors, and hight on pins 2A, and 3A.
	//This puts left motor in reverse, and right motor in forward motion.
	motorSignal = 0x53;
	
}
void moveRight() {
	//Sets enable on both motors, and hight on pins 1A, and 4A.
	//This puts left motor in forward, and right motor in reverse motion.
	motorSignal = 0x35;
}

void stopMotor() {
	//Sets both enables to low.
	motorSignal = 0;
}

void moveBackward() {
	//Sets enable on both motors, and hight on pins 2A, and 4A.
	//This puts motors into reverse.
	motorSignal = 0x55;
}



void TimerOn() {
	TCCR1B = 0x0B;
	
	OCR1A = 125;
	
	TIMSK1 = 0x02;
	
	TCNT1 = 0;
	
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
	
}

void TimerOff () {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}

void TimerSet( unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
ISR(INT0_vect) {
	
	if(measuring) {

		if (UltraFlag) {
			//status = 0x00;
			TCCR0B = 0;
			pulse = (counter * 255) + TCNT0;;
			distance = (pulse/8) / 58;
			if(!(pulse < 3500)) {
				pulse = 0;
				UltraFlag = 0;
				counter = 0;
				TCNT0 = 0;
				measuring = 0;
				oldDistance = distance;
			}
			else {
				pulse = 0;
				UltraFlag = 0;
				counter = 0;
				TCNT0 = 0;
				measuring = 0;
				distance = oldDistance;
			}
			
			
			
		}
		else {
			TCNT0 = 0;
			UltraFlag = 1;
			TCCR0B|= 0x01;
			
		}
	}
}
ISR(TIMER0_OVF_vect) {
	if (UltraFlag) {
		counter++;
		uint32_t ticks = counter * 255 + TCNT0;
		uint32_t max_ticks = (uint32_t)MAX_RESP_TIME_MS * INSTR_PER_MS;
		if (ticks > max_ticks) {
			UltraFlag = 0;          // stop counting timer values
			measuring = 0; // ultrasound scan done
			counter = 0;
			TCCR0B = 0;
			pulse = 0;
			TCNT0 = 0;
			distance = 400;
			oldDistance = 400;
			
		}
	}
	TIFR0 = 0x01;
}

void initUltra() {
	TIMSK0 = 0x01;
	//TIFR0 = 0x01;
	SREG |= 0x80;
	EIMSK |= 0x01;
	EICRA = 0x01;
	sei();
}

void recieveData()
{
	while((UCSR0A &(1<<RXC0)) == 0);
	input = UDR0;
	if (input == '5') mode = 1;
	else if (input == '6') mode = 2;
	else if (input == '7') mode = 3;
	else move = input;

}

void initUSART() {
	cli();
	UBRR0L = BAUD_PRESCALE;
	UBRR0H = (BAUD_PRESCALE >> 8);
	
	UCSR0B = ((1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0));
	sei();
}
ISR(USART0_RX_vect)
{
	recieveData();
}

void sendUSART(uint8_t data) {
	while((UCSR0A & (1<<UDRE0)) == 0);
	
	UDR0 = data;
}