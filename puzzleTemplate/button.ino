#include <Arduino.h>

static int ledPin;
static int buttonPin;
static bool lastValue;

void setupButton()
{
    ledPin = A1;
    buttonPin = A2;
    lastValue = true;
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    digitalWrite(buttonPin, HIGH);
}

bool buttonPressed()
{
    return not digitalRead(buttonPin);
}

void buttonOff()
{
    digitalWrite(ledPin, LOW);
}

void buttonOn()
{
    digitalWrite(ledPin, HIGH);
}