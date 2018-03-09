/*look at EEMPROMEx for really good functions*/

/*get Wifi and colour settings locations from EEPROM
fetch the following from EEPROM: 
wifiSettings_e - do we use defualt or get from SD. EEPROM
colourSettings_e  - do we use defualt or get from SD. EEPROM
ledBrightness - what was the last brightness at ?
*/
void initSystem() {
	Serial.begin(115200);
	/*get last mode and brightness form EEPROM*/
	//gPatternNumber = EEPROM.readByte(EEMPROM_MODE_ADDR);
	//if (gPatternNumber == 0xFF)
	//{
	//	gPatternNumber = 0;
	//	EEPROM.updateByte(EEMPROM_MODE_ADDR, 0); //addr, val
	//}
	//ledBrightnessSin = EEPROM.readByte(EEPROM_BRIGHTNESS_ADDR);

	gPatternNumber = 0;
	ledBrightnessSin = 64;

	gHue = stringToByte(WiFi.macAddress().charAt(15), WiFi.macAddress().charAt(16));
	currentPalette = CRGBPalette16(CHSV(gHue, 192, random8(128, 255)), CHSV(gHue + random8(8), 255, random8(128, 255)), CHSV(gHue + random8(16), 192, random8(128, 255)), CHSV(gHue + random8(32), 255, random8(128, 255)));
	//targetPalette = CRGBPalette16(CHSV(gHue+random(32), 192, random8(128, 255)), CHSV(gHue + random8(64), 255, random8(128, 255)), CHSV(gHue + random8(16), 192, random8(128, 255)), CHSV(gHue + random8(32), 255, random8(128, 255)));

	// disable bluetooth
	 esp_bt_controller_disable();
	 esp_wifi_set_ps(WIFI_PS_MODEM);

	Serial.println(F(FIREFLY_PROJECT));
}

/*get a colour palette from EEPROM*/
void getColorPaletteEEPROM() {

}
/*save a colour palette to EEPROM*/
void saveColorPaletteEEPROM() {

}

