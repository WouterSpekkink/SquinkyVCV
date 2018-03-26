
#include "asserts.h"
#include "MidiViewport.h"

#include "MidiSong.h"

static void test0()
{
    MidiViewport vp;
    {
        MidiSongPtr song( std::make_shared<MidiSong>());
        vp._song = song;

        song->createTrack(0);
        auto track = song->getTrack(0);

        MidiEvent ev;
        track->insertEvent(ev);
        assertEvCount(2);
    }
    assertEvCount(0);
}

void testMidiViewport()
{
    assertEvCount(0);
    test0();
}