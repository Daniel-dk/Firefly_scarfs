

/*check the button states and handle any callbacks*/
void buttonTick() {
	userButton.tick();
}


/*initialise the OneButton objects*/
void initButtons() {

	// link the User button functions.
	userButton.attachClick(clickUserButton);
//	userButton.attachDoubleClick(doubleclickUserButton);
	//userButton.attachLongPressStart(longPressStartUserButton);
//	userButton.attachLongPressStop(longPressStopUserButton);
 	userButton.attachDuringLongPress(longPressUserButton);

}



/*Callbacks*/
/* Click User Button once*/
void clickUserButton() {
	Serial.println(F("clickUserButton()"));
	// add one to the current pattern number, and wrap around at the end
	//gPatternNumber = (gPatternNumber++) % MAX_PATTERN_IDX;
	gPatternNumber++;
	gPatternNumber = gPatternNumber % (ARRAY_SIZE(gPatterns));
	Serial.print(F("pattern is now : "));
	Serial.println(gPatternNumber);

//	EEPROM.updateByte(EEMPROM_MODE_ADDR, gPatternNumber); //addr, val
}


/* Click User Button Twice*/
void doubleclickUserButton() {
	Serial.println(F("doubleclickUserButton()"));
}



/*Long press is detected*/
void longPressStartUserButton() {
	Serial.println(F("longPressStartUserButton()"));
}



/*Long press is ongoing*/
void longPressUserButton() {
	 // change systen brightness and wrap around at the end
	ledBrightnessSin++;  // = (ledBrightnessSin + 1) % 255;
	ledBrightness = sin8(ledBrightnessSin);

	FastLED.setBrightness(ledBrightness);
	showLEDs();
	Serial.print(F("Brightness  :"));
	Serial.println(ledBrightness);
	brightnessSet = true;
	brightnessTimeOut = millis() + 2000;
}

/*Long press is ended*/
void longPressStopUserButton() {
	Serial.println(F("longPressStopUserButton()"));
}

