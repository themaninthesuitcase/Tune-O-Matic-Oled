#include "Tuner.hpp"
#include "TunerUI.hpp"

TunerUI ui = TunerUI();
Tuner tuner = Tuner();

// Data storage variables.
byte newData = 0;
byte prevData = 0;

// Freq variables.
unsigned int period;

// Changing the #defines to consts materially affects the detection!
#define HALF_SAMPLE_VALUE 127
#define TIMER_RATE 38462
#define TIMER_RATE_10 TIMER_RATE * 10

// Data storage variables.
unsigned int time = 0; // Keeps time and sends values to store in timer[] occasionally.
#define BUFFER_SIZE 10
int timer[BUFFER_SIZE];  // Storage for timing of events.
int slope[BUFFER_SIZE];  // Storage for slope of events.
unsigned int totalTimer; // Used to calculate period.
byte index = 0;          // Current storage index.
int maxSlope = 0;        // Used to calculate max slope as trigger point.
int newSlope;            // Storage for incoming slope data.

// Variables for decided whether you have a match.
#define MAX_NO_MATCH_VALUE 9
byte noMatch = 0;  // Counts how many non-matches you've received to reset variables if it's been too long.
byte slopeTol = 3; // Slope tolerance - adjust this if you need.
int timerTol = 10; // Timer tolerance - adjust this if you need.

// Variables for amp detection.
unsigned int ampTimer = 0;
byte maxAmp = 0;
byte checkMaxAmp;
byte ampThreshold = 30; // Raise if you have a very noisy signal.
long clippingTimer = 0;

// Clipping indicator variables.
boolean clipping = true;
#define CLIPPING_TIME 5 * TIMER_RATE // This should amount to 2 seconds.

void SetupInterrupt()
{
    cli(); // Disable interrupts.

    // Set up continuous sampling of analog pin 0.

    // Clear ADCSRA and ADCSRB registers.
    ADCSRA = 0;
    ADCSRB = 0;

    ADMUX |= (1 << REFS0); // Set reference voltage.
    ADMUX |= (1 << ADLAR); // Left align the ADC value- so we can read highest 8 bits from ADCH register only

    ADCSRA |= (1 << ADPS2) | (1 << ADPS0); // Set ADC clock with 32 pre-scaler -> 16mHz / 32 = 500kHz.
    ADCSRA |= (1 << ADATE);                // Enable auto trigger.
    ADCSRA |= (1 << ADIE);                 // Enable interrupts when measurement complete.
    ADCSRA |= (1 << ADEN);                 // Enable ADC.
    ADCSRA |= (1 << ADSC);                 // Start ADC measurements.

    sei(); // Enable interrupts.
}

// When new ADC value ready.
ISR(ADC_vect)
{
    prevData = newData; // Store previous value.
    newData = ADCH;     // Get value from A0.
    if (prevData < HALF_SAMPLE_VALUE && newData >= HALF_SAMPLE_VALUE)
    {                                  // if increasing and crossing midpoint
        newSlope = newData - prevData; // Calculate slope
        if (abs(newSlope - maxSlope) < slopeTol)
        { // If slopes are ==
            // Record new data and reset time.
            slope[index] = newSlope;
            timer[index] = time;
            time = 0;
            if (index == 0)
            {                       // New max slope just reset.
                noMatch = 0;
                index++; // Increment index.
            }
            else if (abs(timer[0] - timer[index]) < timerTol && abs(slope[0] - newSlope) < slopeTol)
            { // if timer duration and slopes match
                // Sum timer values.
                totalTimer = 0;
                for (byte i = 0; i < index; i++)
                {
                    totalTimer += timer[i];
                }
                period = totalTimer; // Set period.
                // Reset new zero index values to compare with.
                timer[0] = timer[index];
                slope[0] = slope[index];
                index = 1; // Set index to 1.
                noMatch = 0;
            }
            else
            {            // Crossing midpoint but not match.
                index++; // Increment index.
                if (index > BUFFER_SIZE - 1)
                {
                    reset();
                }
            }
        }
        else if (newSlope > maxSlope)
        { // If new slope is much larger than max slope.
            maxSlope = newSlope;
            time = 0; // Reset clock.
            noMatch = 0;
            index = 0; // Reset index.
        }
        else
        {              // Slope not steep enough.
            noMatch++; // Increment no match counter.
            if (noMatch > MAX_NO_MATCH_VALUE)
            {
                reset();
            }
        }
    }

    // If clipping
    if (newData == 0 || newData == 1023)
    {
        PORTB |= B00100000; // set pin 13 high, i.e. turn on clipping indicator led.
        clipping = true;    // Currently clipping.
    }

    time++; // Increment timer at rate of 38.5kHz
    clippingTimer++;
    if (clippingTimer > CLIPPING_TIME)
    {
        PORTB &= B11011111; // Set pin 13 low, i.e. turn off clipping indicator led.
        clipping = false;   // Currently not clipping.
        clippingTimer = 0;
    }

    ampTimer++; // Increment amplitude timer.
    if (abs(HALF_SAMPLE_VALUE - ADCH) > maxAmp)
    {
        maxAmp = abs(HALF_SAMPLE_VALUE - ADCH);
    }
    if (ampTimer == 1000)
    {
        ampTimer = 0;
        checkMaxAmp = maxAmp;
        maxAmp = 0;
    }
}

void reset()
{                 // Clear out some variables.
    index = 0;    // Reset index.
    noMatch = 0;  // Reset match counter.
    maxSlope = 0; // Reset slope.
}

void setup()
{
    if (!ui.Start())
    {
        for (;;)
        {
        } // Don't proceed, loop forever
    }

    SetupInterrupt();
}

void loop()
{
    // Get the frequency x10 ie 440Hz gives 4400.
    uint16_t frequency = TIMER_RATE_10 / period; 

    Match match = tuner.CalculateNote(frequency);
    ui.DisplayNoteMatch(match);

    delay(70);
}
