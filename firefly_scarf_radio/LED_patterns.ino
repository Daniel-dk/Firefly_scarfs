
/*LED patterns / animations for addressable strip*/
void LEDanimations() {
	// TODO : // keep the framerate modest

	// Call the current pattern function once, updating the 'leds' array
	gPatterns[gPatternNumber]();

	//EVERY_N_MILLISECONDS(10) {
	FastLED.show(); //showLEDs(); // uses PWM pins and Fastled.show()
	//} // framerate control

	// insert a delay to keep the framerate modest
	 FastLED.delay(1000 / FRAMES_PER_SECOND);
	// do some periodic updates
	EVERY_N_MILLISECONDS(100) { 
		if (blending) {	
			if (abs(gHue - radioMessage_rx.OriginColour) <= 4) {
				blending = false;

			//	Serial.println("blend done");
			}
			else {
				CHSV fromCol,toCol,blendedCol;
				fromCol = CHSV(gHue,255,255);
				toCol = CHSV(radioMessage_rx.OriginColour, 255, 255);
/*
				Serial.print("blend from ");
				Serial.print(gHue);
				Serial.print(" to ");
				Serial.print(radioMessage_rx.OriginColour);
				*/
				blendedCol = blend(fromCol, toCol, 64);

				gHue = blendedCol.hue;
				//gHue = blend8(gHue, radioMessage_tx.OriginColour, 4);
				/*
				Serial.print(":");
				Serial.println(gHue);
			*/
				currentPalette = CRGBPalette16(CHSV(gHue, 192, random8(128, 255)), CHSV(gHue + random8(8), 255, random8(128, 255)), CHSV(gHue, 192, random8(128, 255)), CHSV(gHue + random8(8), 255, random8(128, 255)));

			}
		}
		else {
			if (!fireflyMode) {
				gHue++;
			}	
			fireflyMode = false;
		}
		 
	
	
	} // slowly cycle the "base color" through the rainbow


	// send the 'leds' array out to the actual LED strip
	
	 // FastLED.delay(1000 / FRAMES_PER_SECOND);
	//EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow

}

/*LED patterns - they must be non- blocking - do not use "delay" in them*/
/*default set is from  FastLed's 100 line demoreel  example*/

/* Rainbow */
void solidShimmer() {
	// FastLED's built-in rainbow generator
	fill_solid(leds, numLEDs,CHSV(gHue,255,255));
	if (random8() < 20) {
		int pos = random16(numLEDs);
		leds[pos] += CHSV(gHue, random8(200), 255);
	}
}

/* Rainbow with glitter */
void rainbow() {
	// built-in FastLED rainbow, plus some random sparkly glitter
	fill_rainbow(leds, numLEDs, gHue, 1);
}

/*Confetti*/
void sparkle() {
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy(leds, numLEDs, 10);
	if (random8() < 80) {
		int pos = random16(numLEDs);
		leds[pos] += CHSV(gHue + random8(64), random8(100), 255);
	}
}

/*sinelon*/
void sweepingDot() {
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy(leds, numLEDs, 5);
	int pos = beatsin16(13, 0, numLEDs);
	leds[pos] += CHSV(gHue, 255, 192);
}

/*meteor*/
void bpm() {


	//// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	uint8_t BeatsPerMinute = 44;
   
	uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
	for (int i = 0; i < numLEDs; i++) { 
		// leds[i] = CHSV( gHue + (i * 2), beat - gHue + (i * 10),255 );
		leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
	}
}

/*juggle*/
void dotWeave() {
	// 5 colored dots, weaving in and out of sync with each other
	fadeToBlackBy(leds, numLEDs, 20);
	byte dothue = 0;
	for (int i = 0; i < 5; i++) {
		//leds[beatsin16((i+7)%32, 0, numLEDs)] |= CHSV(dothue,  add8(10, (i * 50)), 255);
		leds[beatsin16((i + 7) % 32, 0, numLEDs)] |= ColorFromPalette(palette, dothue + (i * 2));//
		dothue += 32;
	}
}
void pattern_6() {
//	colourSweep();
 breathe();

}


void breathe() {
//	float val = (exp(sin(millis() / 2000.0*PI)) - 0.36787944)*108.0;
	//uint8_t intVal = val;
	uint8_t intVal = sin8(syncedMillis >>5);

	for (int i = 0; i < numLEDs; i++)
	{
		//leds[i] = CHSV(gHue,100, intVal);
		leds[i] = ColorFromPalette(OceanColors_p,gHue, intVal);
	}
}


/*a more complex Meteor system

*/
void meteorShower() {
	meteorShower(random8(50));
}

void meteorShower(uint8_t chance) {
	// persistent "global" like variables
	static boolean addingMeteor = false; // are we busy adding a meteor ?

	static uint8_t cMeteor = 0; // what type is it ? ( fade white, fade black, fade rainbow )
	static uint8_t cMeteorLen = 0;  //how much more needs t be drawn ?
	static uint8_t cMeteorFullLen = 0;  //how long is this meteor ?
	static CHSV cMeteorCol = CHSV(0,0,0);  //what colour does it start with ?

	// slide all the pixels down one in the array ( away from injection point )
	memmove8(&leds[1], &leds[0], (numLEDs - 1) * 3);
	//CHSV tempcol;
	if (addingMeteor == true) // busy adding one ?
	{
		//tempcol = cMeteorCol;

		if ( cMeteorLen <= 1  ) // end of meteor , allowed to add another one
		{
			leds[0] = CRGB::Black;
			addingMeteor = false;
		}
		else if (cMeteorFullLen == cMeteorLen) // start of meteor
		{
			if (cMeteor <= 3) // 0,1,2 have colour heads, 
			{
				leds[0] = cMeteorCol;
				cMeteorLen--;
			}
			else if (cMeteor == 4)// 4 has a white head
			{
				cMeteorCol.sat = 0;
				leds[0] = cMeteorCol;
				cMeteorLen--;
			}
			
		}
		else // busy with "body"
		{
			double decay = ((double)cMeteorLen / (double)cMeteorFullLen);
			cMeteorLen--;
			switch (cMeteor)
			{
				// Colour head
			case 0 : // fade to dark
				cMeteorCol.val = (double )cMeteorCol.val*(decay);
				leds[0] = cMeteorCol;
				break;
			case 1 : // fade to white + dark
				cMeteorCol.sat = (double)cMeteorCol.sat*(decay);
				cMeteorCol.val = (double)cMeteorCol.val*(decay);
				leds[0] = cMeteorCol;
				break;
			case 2 : // rainbow+fade to dark
				cMeteorCol.val = (double)cMeteorCol.val*(decay);
				cMeteorCol.hue -= 16;
				leds[0] = cMeteorCol;
				break;
			case 3: // rainbow+fade to dark
				cMeteorCol.val = (double)cMeteorCol.val*(decay);
				cMeteorCol.hue -= +16;
				leds[0] = cMeteorCol;
				break;
				// **** White head
			case 4: // white head rainbow+fade to dark
				cMeteorCol.val = (double)cMeteorCol.val*(decay); // brightness 
				cMeteorCol.sat = (double)(255)*( decay);
				cMeteorCol.hue -= 12;
				leds[0] = cMeteorCol;
				break;
			default:
				leds[0] = CRGB::Black;
				break;
			}

		}
	}
	else // we're allowed to add a meteor ( this does the timeout )
	{
		if (random8() < chance) {
			addingMeteor = true;
			cMeteor = random8(5);
			
			cMeteorLen = random8(numLEDs-1);
			cMeteorFullLen = cMeteorLen;
			cMeteorCol = CHSV(random8(),255,255);// color
			meteorSpeed =  random8(30, 200);
		}
	}
	FastLED.delay(1000 / meteorSpeed);

}

void fireShimmer() {
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy(leds, numLEDs, 1);
	if (random8() < 220) {
		int pos = random16(numLEDs);
		leds[pos] += ColorFromPalette(HeatColors_p, random8());
	}
}

void fireShimmerTwo() {
	// random colored speckles that blink in and fade smoothly
	 fadeToBlackBy(leds, numLEDs, 2);
	 blur1d(leds, numLEDs, 2);
	if (random8() < 200) {
		int pos = random16(numLEDs);
		leds[pos] += HeatColor( random8());
	}
}

void colourSweep() {
	static uint8_t currentHue;
	static boolean dir = true;
	static uint16_t pos = 0;

	if (dir == true) // going "up"
	{
		if ((pos < numLEDs))
		{
			leds[pos++] = CHSV(currentHue, 200, 255);
		}
		else {
			dir = false;
			currentHue = gHue;
			pos = numLEDs - 1;
		}
	}
	else { // going "down"
		if (pos>0)
		{
			leds[pos--] = CHSV(currentHue, 200, 255);
		}
		else
		{
			dir = true;
			gHue += 5;
			currentHue = gHue;
			pos = 0;
		}
	}

	fadeToBlackBy(leds, numLEDs, 8);
	//FastLED.delay(1000 / 30);
}


/*some "primitive" animatiosn used in others*/
void addGlitter(fract8 chanceOfGlitter)
{
	if (random8() < chanceOfGlitter) {
		leds[random16(numLEDs)] += CRGB::White;
	}
}

/*Confetti*/
void firefly_blink() {
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy(leds, numLEDs, 10);
	fireflyMode = true;
	//if (random8() < 5) {
	if (random(0,255) < 5) {


		//fill_solid(leds, 5, ColorFromPalette(ForestColors_p, gHue)   );
		//fill_solid(leds+ numLEDs-5, 5, ColorFromPalette(ForestColors_p, gHue)  );
	
		//fill_solid(leds, 5, CRGB::LawnGreen);
		//fill_solid(leds+ numLEDs-5, 5, CRGB::LawnGreen);
		//gHue/=2;
		//Serial.println(gHue);
		fill_solid(leds, 5, CHSV(gHue,200,255) );
		fill_solid(leds+ numLEDs-5, 5, CHSV(gHue,200,255) );
	}
}



void SimpleMeteorShower() {
	static uint16_t idex = 0; //
	// show, delay
	//delay(100);
	// slide all the pixels down one in the array ( away from injection point )
	memmove8(&leds[1], &leds[0], (numLEDs - 1) * 3 );

	// increment the meteor display frame
	idex++;
	// make sure we don't drift into space
	if (idex > 9) {
		//idex = 0;

		if (idex>=numLEDs){
			idex = 0;
		}
		else if (random8() < 10) {
			idex = 0;
		}

		// cycle through hues in each successive meteor tail
		 // gHue += 32;
		//gHue = random8();
	}

	// this switch controls the actual meteor animation, i.e., what gets placed in the
	// first position and then subsequently gets moved down the strip by the memmove above
	switch (idex) {
	case 0:
	//	leds[0] = CRGB(gHue, 0, 200);
		break;
	case 1:
		leds[0] = CHSV((gHue /*- 20*/), 250, 250);
		break;
	case 2:
		leds[0] = CHSV((gHue /*- 22*/), 180, 180);
		break;
	case 3:
		leds[0] = CHSV((gHue /*- 23*/), 150, 150);
		break;
	case 4:
		leds[0] = CHSV((gHue /*- 24*/), 110, 110);
		break;
	case 5:
		leds[0] = CHSV((gHue/* - 25*/), 90, 90);
		break;
	case 6:
		leds[0] = CHSV((gHue/* - 26*/), 60, 60);
		break;
	case 7:
		leds[0] = CHSV((gHue/* - 27*/), 60, 60);
		break;
	case 8:
		leds[0] = CHSV((gHue/* - 28*/), 60, 60);
		break;
	case 9:
		leds[0] = CHSV((gHue/* - 29*/), 10, 20);
		break;
	default:
		leds[0] = CRGB::Black;
	}
}

/* Lightning flash 
types :
0 : White flash, fade to dark
2 : White flash, fade to dark + Hue
3 : Colour flash, fade to dark
*/
void lightning(uint8_t flashType, uint8_t decay, uint8_t hue) {
	CHSV col = CHSV(hue, 0, 255);
	switch (flashType) {
	case 0: //White flash, fade to dark
	case 1: // White flash, fade to dark + Hue
		col = CHSV(hue, 0, 255);
		break;
	case 2:
		col = CHSV(hue, 255, 255);
		break;
	default:
		break;
	}
	fill_solid(leds, numLEDs, col);
	leds[numLEDs] = 0xFFFFFF;
	FastLED.show();
	delay(decay);
	fill_solid(leds, numLEDs, 0);
	leds[numLEDs] = 0xFFFFFF;
	FastLED.show();
	delay(decay);
	fill_solid(leds, numLEDs, col);
	leds[numLEDs] = 0xFFFFFF;
	FastLED.show();
	delay(decay);
	// fade
	for (uint8_t i = col.val; i > 0; i--) {
		switch (flashType) {
		case 0:
			// White flash, fade to dark
			col.v=i;
			break;
		case 1:
			// White flash, fade to dark + Hue
			col.sat = 255 - i;
			col.v = i;
			break;
		case 2:
			// Colour flash, fade to dark
			col.sat = 255;
			col.v = i;
			break;
		default:
			break;
		}
		fill_solid(leds, numLEDs, col);
		leds[numLEDs] = 0xFFFFFF;
		FastLED.show();
		delay(decay);
	}
}


void lightning(uint8_t flashType, uint8_t decay) {
	lightning(flashType, decay, gHue);
}

void ripple() {
	EVERY_N_MILLISECONDS(50) {

		for (uint16_t l = 0; l < numLEDs; l++) {
			leds[l] = CHSV(gHue + 64, 255, 50);         // strip.setPixelColor(l, Wheel(currentBg, 0.1));
		}

		if (step == -1) {
			center = random(numLEDs);
			step = 0;
		}

		if (step == 0) {
			leds[center] = CHSV(gHue, 255, 255);         // strip.setPixelColor(center, Wheel(color, 1));
			step++;
		}
		else {
			if (step < maxSteps) {
				//Serial.println(pow(fadeRate, step));

				leds[wrap(center + step)] = CHSV(gHue, 255, pow(fadeRate, step) * 255);       //   strip.setPixelColor(wrap(center + step), Wheel(color, pow(fadeRate, step)));
				leds[wrap(center - step)] = CHSV(gHue, 255, pow(fadeRate, step) * 255);       //   strip.setPixelColor(wrap(center - step), Wheel(color, pow(fadeRate, step)));
				if (step > 3) {
					leds[wrap(center + step - 3)] = CHSV(gHue, 255, pow(fadeRate, step - 2) * 255);     //   strip.setPixelColor(wrap(center + step - 3), Wheel(color, pow(fadeRate, step - 2)));
					leds[wrap(center - step + 3)] = CHSV(gHue, 255, pow(fadeRate, step - 2) * 255);     //   strip.setPixelColor(wrap(center - step + 3), Wheel(color, pow(fadeRate, step - 2)));
				}
				step++;
			}
			else {
				step = -1;
			}
		}
	}
}


int wrap(int step) {
	if (step < 0) return numLEDs + step;
	if (step > numLEDs - 1) return step - numLEDs;
	return step;
}

void one_color_allHSV(int ahue, int abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
	for (int i = 0; i < numLEDs; i++) {
		leds[i] = CHSV(ahue, 255, abright);
	}
}

//noise rainbow
void noise() {

	EVERY_N_MILLISECONDS(100) {                                  // FastLED based non-blocking delay to update/display the sequence.
		plasma();
	}


	EVERY_N_MILLISECONDS(150) {
		uint8_t maxChanges = 24;
		nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
	}


	EVERY_N_SECONDS(60) {                                 // Change the target palette to a random one every 20 seconds.
		//uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
		targetPalette = CRGBPalette16(CHSV(gHue + random8(32), 192, random8(128, 255)), CHSV(gHue + random8(32), 255, random8(128, 255)), CHSV(gHue + random8(32), 192, random8(128, 255)), CHSV(gHue + random8(32), 255, random8(128, 255)));
	}

}


void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.

	int thisPhase = beatsin8(6, -64, 64);                           // Setting phase change for a couple of waves.
	int thatPhase = beatsin8(7, -64, 64);

	for (int k = 0; k<numLEDs; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:

		int colorIndex = cubicwave8((k * 23) + thisPhase) / 2 + cos8((k * 15) + thatPhase) / 2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
		int thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

		leds[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
	}

} // plasma()