#include "Tuner.hpp"

Tuner::Tuner()
{
    sgLowestFrequency = pgm_read_word_near(&sgNotes[0].mLowest);
    sgHighestFrequency = pgm_read_word_near(&sgNotes[sgNoteCount - 1].mHighest);
}

Match Tuner::CalculateNote(int frequency)
{
    Match result = {
        .Frequency = frequency,
        .Note = {'-', 0}, // because gcc bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55227
        .Octave = 0,
        .OutOfRange = false,
        .IsSharp = false,
        .IsFlat = false,
    };

    // check for frequency out of bounds
    if (frequency < sgLowestFrequency)
    {
        result.OutOfRange = true;
        result.IsFlat = true;
        return result;
    }

    if (frequency > sgHighestFrequency)
    {
        result.OutOfRange = true;
        result.IsSharp = true;
        return result;
    }

    // binary search to find note
    int begin = 0;
    int end = sgNoteCount - 1;

    while (begin != end)
    {
        int middle = (begin + end) >> 1;
        const cFreq4 &item = sgNotes[middle];

        if (middle == begin)
        {
            // We found the note, now test its accuracy
            if (frequency < pgm_read_word_near(&item.mLow))
                result.IsFlat = true;
            else if (frequency > pgm_read_word_near(&item.mHigh))
                result.IsSharp = true;
            // else is in tune!

            strcpy_P(result.Note, (char *)item.mNote);
            result.Octave = pgm_read_byte_near(&item.mOctave);

            break;
        }
        if (frequency < pgm_read_word_near(&item.mLowest))
            end = middle;
        else
            begin = middle;
    }

    return result;
}