#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define bit_get(p, m) (((p) >> m) & 1)
#define bit_set(p, m) ((p) |= (1 << m))
#define bit_clear(p, m) ((p) &= ~(1 << m))
#define bit_flip(p, m) ((p) ^= (1 << m))
#define bit_write(c, p, m) (c ? bit_set(p, m) : bit_clear(p, m))

#define LATCH PINB1
#define DATA PINB3
#define CLOCK PINB4
#define BUTTON PINB0

void setup() {
}

void shiftOut(uint8_t val) {
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		char iv = !!(val & (1 << i));

		if (iv)
			PORTB |= _BV(DATA);
		else
			PORTB &= ~_BV(DATA);

		PORTB |= _BV(CLOCK);
		PORTB &= ~_BV(CLOCK);
	}
	PORTB |= _BV(LATCH);
	PORTB &= ~_BV(LATCH);
}

uint8_t numbers[] = {
	0b10000000, // 1
	0b01000010, // 2
	0b11000010, // 3
	0b01011010, // 4
	0b11011010, // 5
	0b01111110  // 6
};

void delay_ms(uint16_t count) {
	while(count--)
		_delay_ms(1);
}

void diceRoll() {
	int dl = 10;
	uint8_t lastNo = -1;
	while (dl < 200) {
		uint8_t no;
		do {
			no = rand() % 6;
		} while (no == lastNo);
		shiftOut(numbers[no]);
		delay_ms(dl);
		dl *= 1.1;
	}
}

int	main(void) {
	setup();
	DDRB = _BV(LATCH) | _BV(DATA) | _BV(CLOCK);
  PORTB = _BV(BUTTON); // enable pull up for button

  for (;;) {
		shiftOut(0);
		while ((PINB >> BUTTON) & 1) {
			_delay_ms(50);
		}
		diceRoll();
		_delay_ms(3000);
		shiftOut(0);
	}
	return 0;
}
