#pragma once

#include <vector>
#include <map>
#include <memory>
#include "SqCommand.h"

#include "MidiEvent.h"

#define _MAP
class MidiTrack
{
public:
    int size() const;
    bool isValid() const;

    void insertEvent(const MidiEvent& ev);
    void deleteEvent(const MidiEvent&);

    /**
     * Returns all events as a vector, so that they may be indexed.
     * Obviously this is rather slow (O(n)), so don't use it for editing.
     */
    std::vector<MidiEvent> _testGetVector() const;
  

    //
#ifdef _MAP
    using container = std::multimap<uint32_t, MidiEvent>;
#else
    using container = std::vector<MidiEvent>;
#endif
    using iterator =container::iterator;
    using const_iterator = container::const_iterator;
    iterator begin()
    {
        return events.begin();
    }
    iterator end()
    {
        return events.end();
    }
    const_iterator begin() const
    {
        return events.begin();
    }
    const_iterator end() const
    {
        return events.end();
    }
   

private:
    container events;



};

using MidiTrackPtr = std::shared_ptr<MidiTrack>;