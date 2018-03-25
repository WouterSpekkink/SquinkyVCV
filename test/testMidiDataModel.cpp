
#include <assert.h>
#include "MidiTrack.h"

static void testCanInsert()
{
    MidiTrack mt;
    MidiEvent ev;
    ev.pitch = 33;
    ev.startTime = 55;
    assert(mt.size() == 0);
    mt.insertEvent(ev);
    assert(mt.size() == 1);

    MidiEvent ev2 = mt._testGetVector()[0];
    assert(ev2 == ev);

    assert(mt.isValid());
}

static void testInsertSorted()
{
    MidiTrack mt;
    MidiEvent ev;
    ev.pitch = 33;
    ev.startTime = 11;

    MidiEvent ev2;
    ev2.pitch = 44;
    ev2.startTime = 1;

    mt.insertEvent(ev);
    mt.insertEvent(ev2);

    auto mv = mt._testGetVector();
    MidiEvent& ev3 = mv.at(0);
    assert(ev3 == ev2);

    ev3 = mv.at(1);
    assert(ev3 == ev);

    assert(mt.isValid());
}

static void testDelete()
{
    MidiTrack mt;
    MidiEvent ev;
    ev.pitch = 33;
    ev.startTime = 11;

    mt.insertEvent(ev);
    mt.deleteEvent(ev);
    assert(mt.size() == 0);
}


static void testDelete2()
{
    MidiTrack mt;
    MidiEvent ev;

    ev.pitch = 33;
    ev.startTime = 11;
    mt.insertEvent(ev);

    ev.pitch = 44;
    mt.insertEvent(ev);

    mt.deleteEvent(ev);     // delete the second one, with pitch 44
    auto mv = mt._testGetVector();

    assert(mt.size() == 1);
    assert(mv[0].pitch == 33);
}



static void testDelete3()
{
    MidiTrack mt;
    MidiEvent ev;

    ev.pitch = 44;
    ev.startTime = 11;
    mt.insertEvent(ev);

    ev.pitch = 33;
    mt.insertEvent(ev);

    ev.pitch = 44;
    mt.deleteEvent(ev);     // delete the first one, with pitch 44
    auto mv = mt._testGetVector();

    assert(mt.size() == 1);
    assert(mv[0].pitch == 33);
}


void testMidiDataModel()
{
    testCanInsert();
    testInsertSorted();
    testDelete();
    testDelete2();
    testDelete3();
}