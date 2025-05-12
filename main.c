#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

// TESTOWE ---------------------------------------------------------------------------------

void sprawdzaj_diody() {
    while(1) {
        PORTB |= 0b00000001;
        _delay_ms(1000);

        PORTB &= 0b11111110;
        _delay_ms(1000);
    }
}

void sprawdzaj_przyciski() {
    while (1) {
        if (PIND & 0b10000000 || PIND & 0b00100000) { // D7 lub D5
    	//if (PIND & 0b10000000) { // D7
            PORTB |= 0b00000001;
            _delay_ms(200);
        }
        else {
            PORTB &= 0b11111110;
            _delay_ms(200);
        }
    }
}

void wlacz_silniki_na_chwile() {
	while(1){
	PORTB ^= 0b00000001;
//	_delay_ms(1000);
    _delay_ms(1000);
    PORTB |= 0b00000110;
    PORTD = 0b00000101;
    _delay_ms(10000);
    PORTB &= 0b11111001;
    PORTD = 0b00000000;
    _delay_ms(1000);
	}
}

void steruj_jednym_silnikiem_przyciskami() {
    PORTD |= 0b00000001;
    PORTB |= 0b00000010;
    while (1) {
        // Zmiana włącz / wyłącz
        if (PIND & 0b10000000) {
            PORTB ^= 0b00000010;
            _delay_ms(5000);
        }
        else {
        	PORTB &= 0b11111101; // og xor
        	_delay_ms(50);
        }

        // Zmiana kierunku obrotu z chwilowym zatrzymaniem
//        if (PIND & 0b10000000) { //zmiana do debugowania
//            if (PORTB & 0b00000010) {
//                PORTB &= 0b11111101;
//                _delay_ms(1000);
//                PORTD ^= 0b00000011;
//                _delay_ms(1000);
//                PORTB |= 0b00000010;
//            } else {
//                PORTD ^= 0b00000011;
//            }
//            _delay_ms(1000);
//        }
    }
}

void czujnik_buzzer(){
	//PORTC |= 0b00001000; wlaczenie buzzera biiiip
	uint16_t distance1, time1;

	while(1) {
		PORTC |= 0b00000010;
	_delay_us(10);
	PORTC &= 0b11111101;
	time1 = 1;//= pulseInHigh();
	distance1 = (time1 * 34) / 1000 / 2;
	_delay_ms(500);

	for (uint16_t i = 0; i < distance1; i++) {
		PORTC |= 0b00001000;
		_delay_ms(100);
		PORTC &= 0b11110111;
		_delay_ms(100);
	}
	_delay_ms(5000);
	}

}


// MAIN PROGRAM ------------------------------------------------------------------------------

uint16_t distance, time;

bool mode = 0; // 0 - manual  |  1 - auto

void timer1_init()
{
    TCCR1A = 0;               // Normal mode
    TCCR1B = (1 << CS11);     // Prescaler 8 → 0.5 µs ticks at 16 MHz
    TCNT1 = 0;                // Reset counter
}

void buzz_on(){
	PORTC |= 0b00001000;
}

void buzz_off() {
	PORTC &= 0b11110111;
}

uint16_t pulseInHigh()
{
    // Wait for pin to go HIGH
    while (!(PINC & 0b00000100));
    TCNT1 = 0;  // Reset timer

    // Wait while pin is HIGH
    while (PINC & 0b00000100) {
        if (TCNT1 > 60000) break;  // Timeout to avoid infinite loop
    }
    PORTC &= 0b11110111;

    uint16_t ticks = TCNT1;
    return ticks / 2;  // Convert to microseconds (since 1 tick = 0.5 µs)
}

void measure_distance() {
	PORTC |= 0b00000010;
	_delay_us(10);
	PORTC &= 0b11111101;
	time = pulseInHigh();
	distance = (time * 34) / 1000 / 2;
}

// Returns true if mode button is pressed.
bool button_mode_pressed() {
    if (PIND & 0b10000000) { //D7
        return true;
    }
    else {
        return false;
    }
}

// Returns true if shoot button is pressed.
bool button_shoot_pressed() {
    if (PIND & 0b00100000) { //D5
        return true;
    }
    else {
        return false;
    }
}

// Check if port button pressed (or manual flag change has been made) and turn the diode on/off.
void mode_update() {
    if (button_mode_pressed()) {
        mode = !mode;

        if (mode) {
            PORTB |= 0b00000001;
        }
        else {
            PORTB &= 0b11111110;
        }

        _delay_ms(1000);
    }
}

// Turn engine A on (the one for rollers).
void a_enable() {
    PORTB |= 0b00000010; // B1 on
}

// Turn engine A off (the one for rollers).
void a_disable() {
    PORTB &= 0b11111101; // B1 off
}

// Turn engine B on (the one for loading).
void b_enable() {
    PORTB |= 0b00000100; // B2 on
}

// Turn engine B off (the one for loading).
void b_disable() {
    PORTB &= 0b11111011; // B2 off
}

void a_spin_left() {
	PORTD &= 0b11111100;
	PORTD |= 0b00000001;
}

void a_spin_right() {
	PORTD &= 0b11111100;
	PORTD |= 0b00000010;
}

void a_stop_spin() {

}

void b_spin_left() {
	PORTD &= 0b11110011;
	PORTD |= 0b00001000;
}

void b_spin_right() {
	PORTD &= 0b11110011;
	PORTD |= 0b00000100;
}

void b_stop_spin() {

}

// Da ting goes SKRRA!
void shoot() {
	// open the detent
    b_enable();
    a_enable();
    a_spin_left();
    _delay_ms(3000);
    a_spin_right();
    _delay_ms(4000);
    b_disable();
    a_disable();
}

void shoot2(){
	// open the detent
	//b_enable();
	//b_spin_right();

	// ball zium
	a_enable();
	a_spin_right();
	//_delay_ms(3000);

	// close the detent
	//b_disable();
	_delay_ms(200);
	//b_enable();
	//b_spin_right();
	_delay_ms(3000);
	//b_disable();

	_delay_ms(1000);
	// ball ziumed
	a_disable();
}

void shoot3(){
	b_enable();
	b_spin_left();
	_delay_ms(3000);
	b_disable();
	_delay_ms(1000);

	a_enable();
	a_spin_right();
	b_enable();
	b_spin_right();
	_delay_ms(4000);

}

// Automatic mode routine.
void automatic() {
	measure_distance();
	_delay_ms(500);
	if (distance > 20 && distance < 40) {
		buzz_on();
		_delay_ms(1000);
		buzz_off();
		shoot2();
		_delay_ms(3000);
	}
//	else if (distance >= 40 && distance < 100) {
//		buzz_on();
//		_delay_ms(50);
//		buzz_off();
//	}
}

// Manual mode routine.
void manual() {
    if (button_shoot_pressed()) {
        shoot2();
    }
}

// Main work mode.
void terminate() {
    while (1) {
        mode_update();
        if (!mode) {
            manual();
        }
        else {
        	automatic();
        }
    }
}

// Initializing ports
void init() {
//	Port B ----------------------------------------------------------------------------------

		// Port B pin modes: B3-B7 = programmer, B0-B2 = output (diode and engine enable)
		DDRB &= 0b11111000;
		DDRB |= 0b00000111;
		// DDRB &= 0b11000000;
		// DDRB |= 0b00010111;
		// DDRB  = 0bXX010111;

		// Port B default output: Clearing pins B0-B2, the rest belongs to programmer (not touching it)
		PORTB &= 0b11111000;

//	Port C ----------------------------------------------------------------------------------

		// Port C pin modes: only inputs from sensors
		DDRC = 0b00001010;
		// (TRIG output not set yet btw.)

//	Port D ----------------------------------------------------------------------------------

		// Port D pin modes: D0-D3 = engine direction control, D4-D7 = buttons O/I/O/I
		DDRD = 0b01011111;

		// Port D default output: D4 and D6 = 1 (button-output)
		PORTD = 0b01010000;
		// (Engine spin direction is not set yet here btw.)

	timer1_init();
}

int main(void)
{
    init();
//    terminate();

//    sprawdzaj_diody();
//    sprawdzaj_przyciski();
//    buzz_on();
//    buzz_off();

//
//    b_enable();
//    b_spin_left();
    //b_enable();
//
    a_enable();
    a_spin_right();

//
//    while(1) {
//    	automatic();
//    }
   // b_disable();


}
