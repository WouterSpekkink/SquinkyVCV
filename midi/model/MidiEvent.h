#pragma once

#include <stdint.h>

class MidiEvent
{
public:
    uint8_t pitch;      // TODO: why limit us to MIDI range?
    int32_t startTime;

    bool operator == (const MidiEvent&) const;
    bool isValid() const;
};

inline bool MidiEvent::operator == (const MidiEvent& other) const
{
    return pitch == other.pitch &&
        startTime == other.startTime;
}

inline bool MidiEvent::isValid() const
{
    return pitch <= 0x7f &&
        startTime >= 0;
}