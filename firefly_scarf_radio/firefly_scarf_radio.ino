
/*information about this sketch*/
/*Library imports*/
/*ESP32 libraries*/
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi_types.h>

#include <esp_bt.h>
#include <esp_pm.h>
/* FastLED LED control library*/
#include <FastLED.h>		
/* OneButton library to handle callbacks and button press handling */
#include <OneButton.h>		
/*EEPROMEx library for EEPROM usage*/
// #include <EEPROMEx.h>
/* Project specific header */
#include "firelfy_scarf_header.h"

#define FIREFLY_PROJECT "Firefly scarf Radio"

#if (numLEDs > MAX_LEDS)
#error "numLEDs is too large : > MAX_LEDS , please ensure it is smaller then MAX_LEDS"
#endif

/* initialise the patter list */
SimplePatternList gPatterns = { noise, ripple, solidShimmer, rainbow, dotWeave, breathe, sparkle, bpm ,fireShimmerTwo,meteorShower,colourSweep,firefly_blink };

/*setup code here */
void setup()
{
	/* init system information ( structs and global settings values) from EEPROM
	Serial @ 115200 bps*/
	initSystem();

/* link button press callbacks */
	initButtons();

	/* initialise pins that are not initialised by libraries */
	initGPIO();
/* init LED controller  */
/*- Default to WS2812B/NEOPIXEL @ 800kHz */
	FastLED.addLeds<WS2812B, PIXEL_DATA,GRB>(leds, numLEDs).setCorrection(TypicalLEDStrip);;
	//controller = &(FastLED.addLeds<WS2812B, PIXEL_DATA, GRB>(leds, numLEDs));
	
	/* -ALternate init for WS2801 "SPI" like LEDs*/
//	FastLED.addLeds<WS2801, PIXEL_DATA,PIXEL_CLOCK>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);;
/* -ALternate init for APA102 "SPI" like LEDs*/
//	FastLED.addLeds<APA102, PIXEL_DATA,PIXEL_CLOCK>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);;
// there are many more FastLed supported controllers - there are our most common ones
	delay(2000);

	/*init radio*/
	WiFi.mode(WIFI_STA);
	Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
	// Init ESPNow with a fallback logic
	InitESPNow();

	// Once ESPNow is successfully Init, we will register for Send CB to
	// get the status of Trasnmitted packet
	esp_now_register_send_cb(OnDataSent);
	esp_now_register_recv_cb(OnDataRecv);

	// add a broadcast peer
	initBroadcastPeer();
/*set LED brightness scaling ( 0- 255 ) -Default is Maximum brightness*/
	
	ledBrightness = sin8(ledBrightnessSin);

	FastLED.setBrightness(ledBrightness);
	set_max_power_in_volts_and_milliamps(5, 500); // 5V, 500mA
	syncedMillis =0;
	delay(2000);
}

/* main program code here */
void loop()
{
	EVERY_N_MILLIS(10) { syncedMillis+=10; }
/*check button states and handle callbacks*/
//	buttonTick();
/*handle Radio*/
	handleRadio();
/*handle USB Serial*/
	handleSerial();
/*handle LED animations*/
 LEDanimations();
// TODO  : colour palette in EEPROM and selection of that vs full HSV
	//apply brightness setting 2 seconds after the user has set it
	if (brightnessSet) {
		if (after(brightnessTimeOut)) {
	//		EEPROM.updateByte(EEPROM_BRIGHTNESS_ADDR, ledBrightnessSin); //addr, val
			brightnessSet = false;
			Serial.println(F("Brightness applied"));
		}
		
	}
	delay(10);
}

void handleSerial() {
	char c = 0;
	if (Serial.available() > 0)
		c = Serial.read();

		switch (c) {
		case '1' :
			clickUserButton();
			break;
		case '2':
			Serial.println(F("time sync"));
			syncTimeout = millis();
			break;
		case '3':
			Serial.println(F("colour sync"));
			break;
		case '4':
			Serial.println(F("pattern sync"));
			break;
		default:
			break;
		}
}


/**
*Timeout calculator that determines if an event should happen
*
* @param timeout is the scheduled time for an event to happen
* @return a boolean flag ( TRUE or FALSE ) that indicates if a
* timeout has happened or not
*/

boolean after(long timeout)
{
	return smillis() - timeout>0;
}


//unsigned long rand2() {
//	lcg_seed = (lcg_seed * 1103515245L + 12345L) % 2147483648L;
//	return lcg_seed;
//}
//
//uint8_t rand8() {
//	lcg_seed = (lcg_seed * 1103515245L + 12345L) % 2147483648L;
//	return lcg_seed%0xFF;
//}