#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LowPower.h>

#define PIN_SENSOR A1

#define PIN_RELAY_SIGNAL 3
#define PIN_RELAY_RESET 4

#define PIN_TOGGLE 5
#define PIN_STAT 6

#define AVERAGE_TIMES 5
#define AVERAGE_DELAY 25

//#define DEBUG

Adafruit_SSD1306 display(128, 32, &Wire, -1);
bool onBattery = false;
bool manuallySet = false;
bool justSwitched = false;
bool showStat = true;

uint16_t minVoltage = 800;
uint16_t maxVoltage = 930;
uint16_t voltageMap[] = {
	750,
	760,
	770,
	780,
	790,
	800,
	810,
	820,
	830,
	840,
	850,
	860,
	870,
	880,
	890,
	900,
	910,
	920,
	930,
	940
};

static const unsigned char PROGMEM logo_battery[] =
{ 0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b11111111, 0b11111000,
  0b11111111, 0b11111000,
  0b11111111, 0b11111110,
  0b11111111, 0b11111110,
  0b11111111, 0b11111110,
  0b11111111, 0b11111110,
  0b11111111, 0b11111000,
  0b11111111, 0b11111000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000 
};

static const unsigned char PROGMEM logo_live[] =
{ 0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000111, 0b11000000,
  0b00001111, 0b11000000,
  0b00001111, 0b11111000,
  0b11111111, 0b11111000,
  0b11111111, 0b11000000,
  0b11111111, 0b11111000,
  0b00001111, 0b11111000,
  0b00001111, 0b11000000,
  0b00000111, 0b11000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000
};

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Initializing...!");
#endif
	delay(500);

	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		Serial.println(F("SSD1306 allocation failed"));
		for (;;);
	}

	display.clearDisplay();
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(25,18);
	display.println(F("Please Wait..."));
	display.setCursor(0, 0);
	display.display();

	pinMode(PIN_TOGGLE, INPUT_PULLUP);
	pinMode(PIN_STAT, INPUT_PULLUP);

	pinMode(PIN_RELAY_SIGNAL, OUTPUT);
	digitalWrite(PIN_RELAY_SIGNAL, 1);
	pinMode(PIN_RELAY_RESET, OUTPUT);
	digitalWrite(PIN_RELAY_RESET, 1);

	delay(100);
	digitalWrite(PIN_RELAY_RESET, 0);
	delay(100);
	digitalWrite(PIN_RELAY_RESET, 1);

#ifdef DEBUG
	Serial.println("System active.");
#endif
	display.clearDisplay();
	display.display();
}

void loop() {
	uint16_t sensorValue = getReadingAvg(PIN_SENSOR);
#ifdef DEBUG
	Serial.print("Sensor: ");
	Serial.println(sensorValue);
#endif

	if (onBattery) {
		if (sensorValue < minVoltage) {
			onBattery = false;
			switchState();
			justSwitched = true;
		}
	}
	else {
		if (sensorValue >= maxVoltage) {
			onBattery = true;
			switchState();
			justSwitched = true;
		}
	}

	if (digitalRead(PIN_TOGGLE)) {
		manuallySet = true;
		onBattery = !onBattery;
		switchState();

		display.clearDisplay();
		display.setCursor(25, 18);
		display.print(F("Set To: "));
		if (onBattery) {
			display.println(F("Bat"));
		}
		else {
			display.println(F("Live"));
		}
		display.display();

		LowPower.idle(SLEEP_4S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

		display.clearDisplay();
		display.display();
	}
	if (digitalRead(PIN_STAT)) {
		showStat = !showStat;
	}

	if (showStat || justSwitched) {
		display.clearDisplay();
		if (onBattery) {
			display.drawBitmap(30, 0, logo_battery,15,15, SSD1306_WHITE);
		}
		else {
			display.drawBitmap(30, 0, logo_live, 15, 15, SSD1306_WHITE);
		}
		display.setCursor(50, 0);
		display.setTextSize(2);
		display.print((String)getBatteryPercent(sensorValue));
		display.println(F("%"));
		display.setTextSize(1);
		display.setCursor(0, 15);

		if (onBattery) {
			if (justSwitched) {
				display.println(F("Live => Bat"));
			}
			else {
				display.println(F("Bat"));
			}
		}
		else {
			if (justSwitched) {
				display.println(F("Bat => Live"));
			}
			else {
				display.println(F("Live"));
			}
		}

		display.print(F("Min: "));
		display.print((String)getBatteryPercent(minVoltage));
		display.print(F("%. "));
		display.print(F("Max: "));
		display.print((String)getBatteryPercent(maxVoltage));
		display.println(F("%"));
		display.display();
	}
	else
	{
		display.clearDisplay();
		display.display();
	}

	LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);

	justSwitched = false;
}

int getReadingAvg(uint16_t pin) {
	uint16_t total = 0;
	for (int i = 0; i < AVERAGE_TIMES; i++) {
		total += analogRead(pin);
		delay(AVERAGE_DELAY);
	}
	return total / AVERAGE_TIMES;
}

void switchState() {
	if (onBattery) {
		digitalWrite(PIN_RELAY_SIGNAL, 0);
		delay(100);
		digitalWrite(PIN_RELAY_SIGNAL, 1);
	}
	else {
		digitalWrite(PIN_RELAY_RESET, 0);
		delay(100);
		digitalWrite(PIN_RELAY_RESET, 1);
	}
}

uint8_t getBatteryPercent(uint16_t voltage) {
	uint8_t total = sizeof(voltageMap) / 2;
	uint8_t index = 0;
	for (uint8_t i = 0; i < total; i++) {
		if (voltageMap[i] > voltage)
			break;
		index = i;
	}
	if (index > total)
		index = total;
	uint8_t percent = ((float)index / ((float)total)) * 100;
#ifdef DEBUG
	Serial.print("Percentage: ");
	Serial.print(percent);
	Serial.print(" index: ");
	Serial.print(index);
	Serial.print(" total: ");
	Serial.println(total);
#endif
	return percent;
}