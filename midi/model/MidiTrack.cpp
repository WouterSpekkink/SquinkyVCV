#include <assert.h>
#include <algorithm>
#include "MidiTrack.h"

int MidiTrack::size() const
{
    return (int) events.size();
}

#ifdef _MAP
bool MidiTrack::isValid() const
{
    int32_t startTime = 0;
    for (const_iterator it = begin(); it != end(); ++it) {
        if (!it->second.isValid()) {
            return false;
        }
        if (it->second.startTime < startTime) {
            return false;
        }
        startTime = it->second.startTime;
    }
    return true;
}

void MidiTrack::insertEvent(const MidiEvent& evIn)
{
    events.insert( std::pair<uint32_t, MidiEvent>(evIn.startTime, evIn));
}

#if 0 // simple minded one
void MidiTrack::deleteEvent(const MidiEvent& evIn)
{
    auto insertPoint = events.lower_bound(evIn.startTime);
  //  events.insert(insertPoint, std::pair<uint32_t, MidiEvent>(evIn.startTime, evIn));
    events.erase(insertPoint);      // will never work in the real world
}
#endif

void MidiTrack::deleteEvent(const MidiEvent& evIn)
{
    auto candidateRange = events.equal_range(evIn.startTime);
    for (auto it = candidateRange.first; it != candidateRange.second; it++) {
      
        if (it->second == evIn) {
            events.erase(it);
            return;
        }
    }
    assert(false);
    //  events.insert(insertPoint, std::pair<uint32_t, MidiEvent>(evIn.startTime, evIn));
   // events.erase(insertPoint);      // will never work in the real world
}

std::vector<MidiEvent> MidiTrack::_testGetVector() const
{
    std::vector<MidiEvent> ret;
    std::for_each(events.begin(), events.end(), [&](std::pair<uint32_t, const MidiEvent&> event) {
        ret.push_back(event.second);
        });
    assert(ret.size() == events.size());

    return ret;
}

#else
 
bool MidiTrack::isValid() const
{
    uint32_t startTime = 0;
    for (const_iterator it = begin(); it != end(); ++it) {
        if (!it->isValid()) {
            return false;
        }
        if (it->startTime < startTime) {
            return false;
        }
        startTime = it->startTime;
    }
    return true;
}
void MidiTrack::insertEvent(MidiEvent& evIn)
{
    // look for an event after where we want to insert
    std::vector<MidiEvent>::iterator afterEvent = std::find_if(events.begin(), events.end(), [&evIn](MidiEvent& ev) {
        return ev.startTime > evIn.startTime;
        });

    if (afterEvent == events.end()) {
        events.push_back(evIn);
    } else {
        events.insert(afterEvent, evIn);     // this should insert it before
    }
}

MidiEvent& MidiTrack::at(int index)
{
    return events[index];
}
#endif