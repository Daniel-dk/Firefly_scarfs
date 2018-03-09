#pragma once
/*Pin definitions*/

#define smillis() ((long)millis())

// Use qsuba for smooth pixel colouring and qsubd for non-smooth pixel colouring
#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0



/*Radio config*/
#define CHANNEL 3
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

#define LED_BUILTIN 2

/*--Buttons*/
#define BUTTON_USR 2 // 13 on EVKIT // 6 on firefly

/*--Pixel*/
/*--- Adressable LEDs*/

#define PIXEL_DATA 4 // 2 on evkit, A3 on firefly
#define PIXEL_CLOCK 1


/*--- onboard LED enable ( LED is tied to PWM pins )*/
#define ONBOARD_LED_EN 18
/*--- Maximum adressable LEDs we will allow*/
#define MAX_LEDS 400
/*--- default to supporting the max amount of adressable LEDs*/
#define NUM_LEDS_DEFAULT 145 // 145 leds in furry scarfs ( 1.5 m or 30 led/m )
#define NUM_LEDS_SCARF 45
/*--- default to full brightness range*/
#define DEFAULT_BRIGHTNESS 255
/*--- our maximum framerate that we won't go above*/
#define FRAMES_PER_SECOND  100
/*--- how many animation pattern things we have to cycle thru*/
#define MAX_PATTERN 6
/*--- index of the last pattern*/
#define MAX_PATTERN_IDX (MAX_PATTERN-1)
/*--- used when we cycle thru the patterns*/
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define METEOR_THRESHOLD 15  // higher meanse more meteors, 15 is good for baloooon

/*Structure and Enum definitions*/
#define EEMPROM_MODE_ADDR 0 // 1 byte
#define EEPROM_BRIGHTNESS_ADDR 1 // 1 byte

/*instances of libraries*/

// Setup a new OneButton for User buton, active low.  
OneButton userButton(BUTTON_USR, true);
boolean brightnessSet = false;
uint32_t brightnessTimeOut;
// Define the array of adressable LEDs
CRGB leds[MAX_LEDS];
// LED controller - so we can change the amount of LEDs we speak to up to the MAX
// CLEDController* controller;
/* Global brightness adjustment */
uint8_t ledBrightness = DEFAULT_BRIGHTNESS;
uint8_t ledBrightnessSin = 0;
// how many LEDs we will be using (must be <= MAX_LEDS )
uint16_t numLEDs = NUM_LEDS_SCARF;
/*index for meteors*/
// uint16_t idex = 0;
// List of patterns to cycle through.  Each is defined as a separate function in the LED_patterns.ino
// ATT : if you add more patterns here, adjust MAX_PATTERN accordingly, default is 6 ( 0 to 5 )
typedef void(*SimplePatternList[])();

uint8_t gPatternNumber = 9; // Index number of which pattern is displayed - buttonpress will change this
			  
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
int meteorSpeed = 30;
boolean fireflyMode = false;

int center = 0;
int step = -1;
int maxSteps = 9;
float fadeRate = 0.8;
int diff;

//using palettes is a 768 - byte cost in RAM to build the lookup table.
//CRGBPalette16 currentPalette; // takes up 64 bytes. - 16 entry palette ( will linear intepolate to a 256 entry table)
CRGBPalette16 palette = PartyColors_p;
CRGBPalette16 currentPalette = OceanColors_p;                                 // Palette definitions
CRGBPalette16 targetPalette = OceanColors_p;
TBlendType currentBlending = LINEARBLEND;


/***************************Radio datas**************************/

uint32_t syncedMillis; // timer used for animations
uint32_t syncTimeout = 0;
const uint32_t syncTime = 600000; // should be 10 minutes ( 600000 ) 
char rxMACarr[6];


enum radioCommands_enum
{
	COLOUR_SYNC,
	PATTERN_SYNC,
	ALL_SYNC,
	REBROADCAST,
};
/*radio broadcast peer*/
esp_now_peer_info_t peer;
boolean dataRecvd = false;
uint8_t broadcastCommand = ALL_SYNC;
boolean blending = false; // are we busy blending over to another  colour ?

/*structure of radio messages*/
const int radioMessageStructSize = 5; // bytes
union radiomessage_u {
	struct {
		uint8_t MessageType;	// must be 0x42 because reasons
		uint8_t Command; // command (sync colour, sync animation,)
		uint8_t OriginMode; // Origin mode / animation
		uint8_t OriginColour; // origionator colour ( use to set gHue )	
		uint8_t Checksum; // xor checksum
	};
	uint8_t messageArr[radioMessageStructSize];
}radioMessage_rx, radioMessage_tx;

union uLong_union
{
	unsigned long uLong;
	char uLong_arr[4];
};