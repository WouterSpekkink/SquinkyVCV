#pragma once

#include "MidiTrack.h"

class MidiSong;
/**
 * Provides a description of the data to be displayed in the view.
 */
class MidiViewport
{
public:
   
    MidiTrack::const_iterator begin();
    MidiTrack::const_iterator end();
    // This is all pretty note specific
    int32_t startTime;
    int32_t endTime;
    int pitchLow;
    int pitchHi;
    std::shared_ptr<MidiSong> getSong();

    // Below is not for clients to call. TODO: use private or something.
    // Definitely need some architecture here.
    std::weak_ptr<MidiSong> _song;
};
