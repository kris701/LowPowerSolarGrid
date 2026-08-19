#include <LowPower.h>

#define PIN_SENSITIVITY_UP A1
#define PIN_SENSITIVITY_DOWN A2
#define PIN_SENSOR A3

#define PIN_LED 13

#define PIN_RELAY_SIGNAL 4
#define PIN_RELAY_RESET 3

#define AVERAGE_TIMES 5
#define AVERAGE_DELAY 250

//#define DEBUG

bool isUp = false;

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Initializing...!");
#endif
	delay(4000);

	pinMode(PIN_RELAY_SIGNAL, OUTPUT);
	digitalWrite(PIN_RELAY_SIGNAL, 1);
	pinMode(PIN_RELAY_RESET, OUTPUT);
	digitalWrite(PIN_RELAY_RESET, 1);

	delay(100);
	digitalWrite(PIN_RELAY_RESET, 0);
	delay(100);
	digitalWrite(PIN_RELAY_RESET, 1);

	delay(5000);

#ifdef DEBUG
	Serial.println("System active.");
#endif
}

void loop() {
	uint16_t sensorValue = getReadingAvg(PIN_SENSOR);
	uint16_t sensitivityDownValue = getReadingAvg(PIN_SENSITIVITY_DOWN);
	uint16_t sensitivityUpValue = getReadingAvg(PIN_SENSITIVITY_UP);

#ifdef DEBUG
	Serial.print("Up sense: ");
	Serial.print(sensitivityUpValue);
	Serial.print(". Down sense: ");
	Serial.print(sensitivityDownValue);
	Serial.print(". Sense: ");
	Serial.println(sensorValue);
#endif

	if (isUp) {
		if (sensorValue <= sensitivityDownValue) {
#ifdef DEBUG
			Serial.println("Relay off");
#endif
			isUp = false;
			digitalWrite(PIN_RELAY_RESET, 0);
			delay(100);
			digitalWrite(PIN_RELAY_RESET, 1);
		}
	}
	else {
		if (sensorValue >= sensitivityUpValue) {
#ifdef DEBUG
			Serial.println("Relay on");
#endif
			isUp = true;
			digitalWrite(PIN_RELAY_SIGNAL, 0);
			delay(100);
			digitalWrite(PIN_RELAY_SIGNAL, 1);
		}
	}

	LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

int getReadingAvg(uint16_t pin) {
	uint16_t total = 0;
	for (int i = 0; i < AVERAGE_TIMES; i++) {
		total += analogRead(pin);
		delay(AVERAGE_DELAY);
	}
	return total / AVERAGE_TIMES;
}
