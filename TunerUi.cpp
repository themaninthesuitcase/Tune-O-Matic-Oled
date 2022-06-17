#include "TunerUI.hpp"


TunerUI::TunerUI()
{
    display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

bool TunerUI::Start()
{
    bool result = display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

    if (result)
    {
        display.clearDisplay();
    }

    return result;
}

void TunerUI::DisplayNoteMatch(Match match)
{
    display.clearDisplay();

    // Show the frequency in the top left.
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print(match.Frequency / 10.0);

    DrawNote(match.Note, match.Octave);

    if (match.IsFlat)
        DrawFlatTriangle();
    else if (match.IsSharp)
        DrawSharpTriangle();
    else
        DrawInTune();

    display.display();
}

void TunerUI::DrawNote(char *note, char octave)
{
    int x = (SCREEN_WIDTH - CHAR_WIDTH * FONT_SIZE) / 2;
    int y = (SCREEN_HEIGHT - CHAR_HEIGHT * FONT_SIZE) / 2;

    display.setCursor(x, y);
    display.setTextColor(WHITE);
    display.setTextSize(FONT_SIZE);
    display.print(note);

    x = x - (CHAR_WIDTH * SMALL_FONT);
    display.setCursor(x, y);
    display.setTextSize(SMALL_FONT);
    display.print((int)octave);
}

void TunerUI::DrawFlatTriangle()
{
    int x = FONT_SIZE * CHAR_WIDTH;
    display.fillTriangle(x, TUNE_Y_OFFSET, x, SCREEN_HEIGHT - TUNE_Y_OFFSET, MARGIN, SCREEN_HEIGHT / 2, WHITE);
}

void TunerUI::DrawSharpTriangle()
{
    int x = SCREEN_WIDTH - (FONT_SIZE * CHAR_WIDTH);
    display.fillTriangle(x, TUNE_Y_OFFSET, x, SCREEN_HEIGHT - TUNE_Y_OFFSET, SCREEN_WIDTH - MARGIN, SCREEN_HEIGHT / 2, WHITE);
}

void TunerUI::DrawInTune()
{
    int boxWidth = 3 * FONT_SIZE * CHAR_WIDTH;
    int boxHeight = 2 * FONT_SIZE * CHAR_HEIGHT;
    display.drawRect((SCREEN_WIDTH - boxWidth) / 2, (SCREEN_HEIGHT - boxHeight) / 2, boxWidth, boxHeight, WHITE);
}
