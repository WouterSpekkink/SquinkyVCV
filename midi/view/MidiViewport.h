#pragma once

#include "MidiTrack.h"

class MidiSong;
/**
 * Provides a description of the data to be displayed in the view.
 */
class MidiViewport
{
public:
   
#if 0
    // We need a custom iterator to filter by events
    // (although we will probably want to move it out of this class
    class const_iterator
    {
    public:
        const_iterator(MidiTrack::const_iterator);
        bool operator != (const const_iterator& evenIt) const;
        const_iterator& operator ++ ();
        const_iterator operator ++ (int);
        const_iterator operator -- ();
        const MidiEvent operator * () const;
         MidiEvent operator * ();
        const MidiEvent * operator ->() const;
    private:
        MidiTrack::const_iterator
    };
    using iterator_pair = std::pair<const_iterator, const_iterator>;
#endif

  
    using iterator_pair =MidiTrack::iterator_pair;
    iterator_pair getEvents() const;

    // This is all pretty note specific
    int32_t startTime=0;
    int32_t endTime=0;
    int pitchLow=0;
    int pitchHi=0;
    int track=0;
    std::shared_ptr<const MidiSong> getSong() const;

    // Below is not for clients to call. TODO: use private or something.
    // Definitely need some architecture here.
    std::weak_ptr<MidiSong> _song;
};
