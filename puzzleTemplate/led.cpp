//
// Created by Lasciel on 8/13/2016.
//

#include "led.h"
#include "config.h"

uint8_t gHue = 0; // rotating "base color" used by many of the patterns
CRGB leds[NUM_LEDS];
bool gReverseDirection = false;
int led_state = 0;

CRGB g_cross[6];
CRGB g_cube[6];
int num_colors = 3;
CRGB options[3] = {CRGB::Red, CRGB::Blue, CRGB::Green};
bool pending_spatial_update;

void updateSpaitialPuzzle()
{
    if(pending_spatial_update) {
        for (int i = 0; i < LED_BASE; ++i) {
            leds[i] = CRGB::Black;
        }
        for (int i = 0; i < 6 * 4; ++i) {
            leds[LED_BASE + i] = g_cube[i / 4];
        }

        for (int i = 0; i < 6 * 4; ++i) {
            leds[LED_BASE + i + 6 * 4] = g_cross[i / 4];
        }
        pending_spatial_update = false;
    }
}

void setRandomCross()
{
    for(int i = 0; i < 6; ++i)
    {
        long index = random(num_colors);
        g_cross[i] = options[index];
    }
}

void setRandomCube()
{
    for(int i = 0; i < 6; ++i)
    {
        long index = random(num_colors);
        g_cube[i] = options[index];
    }
}

bool calcMatch(int * r1, int * r2, int len)
{
    // If the length of our "remaining sides"
    // arrays is 0, we have matched all the sides
    // and found a match!
    if(len == 0)
    {
        return true;
    }

    // Holds the opposite face by index.
    // So the 0ths face is opposite face 5
    int opposites[6] = {5,4,3,2,1,0};

    for(int i = 0; i < len; ++i)
    {
        int s1 = r1[i];
        int o1 = opposites[s1];
        for(int j = 0; j < len; ++j)
        {
            int s2 = r2[j];
            int o2 = opposites[s2];

            if (g_cube[s1] == g_cross[s2] && g_cube[o1] == g_cross[o2])
            {
                int newLen = len - 2;
                int newR1[newLen];
                int newR2[newLen];

                int r1Index = 0;
                int r2Index = 0;
                // Add the remaining elements except the ones we found
                for(int k = 0; k < len; ++ k)
                {
                    if(r1[k] != s1 &&
                       r1[k] != o1)
                    {
                        newR1[r1Index] = r1[k];
                        ++r1Index;
                    }

                    if(r2[k] != s2 &&
                       r2[k] != o2)
                    {
                        newR2[r2Index] = r2[k];
                        ++r2Index;
                    }
                }

                if(calcMatch(newR1, newR2, newLen))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void spatialPuzzleNewBox(bool match)
{
    int r1[6] = {5,4,3,2,1,0};
    int r2[6] = {5,4,3,2,1,0};
    setRandomCube();
    while(calcMatch(r1, r2, 6) != match)
    {
        setRandomCube();
        setRandomCross();
    }
    pending_spatial_update = true;
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
////
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation,
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking.
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

static int flashCount;
static bool flashState;
static unsigned long flashTimer;
static CRGB flashColor;
static int flashSetCount;

void Fire2012()
{
// Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
        CRGB color = HeatColor( heat[j]);
        int pixelnumber;
        if( gReverseDirection ) {
            pixelnumber = (NUM_LEDS-1) - j;
        } else {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }
}


void addGlitter( fract8 chanceOfGlitter)
{
    if( random8() < chanceOfGlitter) {
        leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
}

void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy( leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy( leds, NUM_LEDS, 20);
    int pos = beatsin16(13,0,NUM_LEDS);
    leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy( leds, NUM_LEDS, 20);
    byte dothue = 0;
    for( int i = 0; i < 8; i++) {
        leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void flash()
{
    if(ledGetFlashDone())
    {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::Black;
            FastLED.show();
        }
    }
    else {
        if (millis() - flashTimer > LED_FLASH_RATE) {
            for (int i = 0; i < NUM_LEDS; i++) {
                if (flashState) {
                    leds[i] = flashColor;
                } else {
                    leds[i] = CRGB::Black;
                }
            }
            if (!flashState) {
                ++flashCount;
            }
            flashTimer = millis();
            flashState = !flashState;
        }
    }
}

void setFlashCount(int count)
{
    flashSetCount = count;
}

bool ledGetFlashDone()
{
    return flashCount + 1 > flashSetCount;
}

void ledSetState(int state)
{
    led_state = state;
    switch(led_state) {
        case LED_FLASH_GREEN:
        {
            flashState = true;
            flashTimer = millis();
            flashColor = CRGB::Green;
            flashCount = 0;
            break;
        }
        case LED_FLASH_RED: {
            flashState = true;
            flashTimer = millis();
            flashColor = CRGB::Red;
            flashCount = 0;
            break;
        }
        default:
            break;
    }
}

void ledUpdate()
{
    switch(led_state) {
        case LED_OFF:{
            resetLed();
            break;
        }
        case LED_IDLE: {
            rainbowWithGlitter();
            break;
        }
        case LED_VICTORY: {
            Fire2012();
            break;
        }
        case LED_FLASH_GREEN:
        {
            flash();
            break;
        }
        case LED_FLASH_RED:
        {
            flash();
            break;
        }
        case LED_SPATIAL_PUZZLE:
        {
            updateSpaitialPuzzle();
            break;
        }
        default:
            break;
    }
    FastLED.show();
}

void ledSetup() {
    delay(1000); // sanity delay
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
}

void resetLed() {
    for (int i=0; i<NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
}

