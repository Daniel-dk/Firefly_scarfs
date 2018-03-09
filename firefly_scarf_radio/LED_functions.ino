/*low level functions that relate to LED manipulation go here*/



/*Wrapper for Fastled.show so it also outputs led[0] on  on PWM*/
void showLEDs() {
	FastLED.show();
}
