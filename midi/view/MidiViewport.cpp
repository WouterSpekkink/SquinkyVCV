
#include "MidiViewport.h"
#include "MidiSong.h"

std::shared_ptr<const MidiSong> MidiViewport::getSong() const
{
    return _song.lock();
}

MidiViewport::iterator_pair MidiViewport::getEvents() const
{
    const auto song = getSong();
    const auto track = song->getTrack(this->track);

    const auto rawIterators =  track->timeRange(this->startTime, this->endTime);
    return rawIterators;
   
    //return iterator_pair(const_iterator(rawIterators.first), const_iterator(rawIterators.second));
}
