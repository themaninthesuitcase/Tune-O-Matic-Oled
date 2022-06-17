#ifndef _TUNER_UI_H_
#define _TUNER_UI_H_

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Tuner.hpp"

class TunerUI
{
    const int SCREEN_WIDTH = 128;
    const int SCREEN_HEIGHT = 64;
    const int OLED_RESET = 4;
    const int SCREEN_ADDRESS = 0x3C;

    const int CHAR_WIDTH = 6;
    const int CHAR_HEIGHT = 8;

    const int FONT_SIZE = 4;
    const int SMALL_FONT = 2;
    const int MARGIN = 5;
    const int TUNE_Y_OFFSET = (SCREEN_HEIGHT - CHAR_HEIGHT * FONT_SIZE) / 2; // Used to space the  < > to match the letter height

    Adafruit_SSD1306 display;

    void DrawFlatTriangle();
    void DrawSharpTriangle();
    void DrawInTune();
    void DrawNote(char *note, char octave);

public:
    TunerUI();
    bool Start(void);
    void DisplayNoteMatch(Match match);
};

#endif // _TUNER_UI_H_
