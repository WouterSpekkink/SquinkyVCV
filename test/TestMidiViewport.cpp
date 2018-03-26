
#include "asserts.h"
#include "MidiViewport.h"

#include "MidiSong.h"


static void testReleaseSong()
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

static void testEventAccess()
{
    MidiSongPtr song(std::make_shared<MidiSong>());
   

    song->createTrack(0);
    auto track = song->getTrack(0);

    MidiEvent ev;
    ev.startTime = 100;
    ev.pitch = 40;
    track->insertEvent(ev);

    MidiViewport vp;
    vp._song = song;
    vp.startTime = 90;
    vp.endTime = 110;
    vp.pitchLow = 0;
    vp.pitchHi = 80;

    auto its = vp.getEvents();
    assertEQ(std::distance(its.first, its.second), 1);

    assert(its.first != its.second);

    auto i = its.first;
    auto x = i->second.startTime;
    its.first++;

}


static void testEventFilter()
{
    MidiSongPtr song(std::make_shared<MidiSong>());


    song->createTrack(0);
    auto track = song->getTrack(0);

    MidiEvent ev;
    ev.startTime = 100;
    ev.pitch = 40;
    track->insertEvent(ev);
    ev.startTime = 100;
    ev.pitch = 50;
    track->insertEvent(ev);

    MidiViewport vp;
    vp._song = song;
    vp.startTime = 90;
    vp.endTime = 110;
    vp.pitchLow = 0;
    vp.pitchHi = 45;

    auto its = vp.getEvents();
    assertEQ(std::distance(its.first, its.second), 1);

}

void xx();
void testMidiViewport()
{
    xx();
    assertEvCount(0);
    testReleaseSong();
    testEventAccess();
    //testEventFilter();
    printf("TODO: event filter\n");
    assertEvCount(0);
}

//first try, can iterator over array

class C
{
public:
    int pp;
    C(int q) : pp(q)
    {
    }
};

using viter = std::vector<C>::iterator;
#if 0
// typedef ???? difference_type; //almost always ptrdiff_t
typedef ? ? ? ? value_type; //almost always T
typedef ? ? ? ? reference; //almost always T& or const T&
typedef ? ? ? ? pointer; //almost always T* or const T*
typedef ? ? ? ? iterator_category;  //usually std::forward_iterator_tag or similar
#endif
class iter1
{
public:
    iter1(viter v) : _orig(v)
    {
    }
    bool operator != (const iter1& z) const
    {
        return z._orig != _orig;
    }
    iter1& operator ++ ()
    {
        _orig++;
        return *this;
    }
    iter1 operator ++ (int)
    {
        ++_orig;
    }
    iter1 operator -- ();

    C operator * () const
    {
        return *_orig;
    }
    const C * operator ->() const
    {
        return &*_orig;
    }

    typedef ptrdiff_t difference_type;
    typedef int value_type;
    typedef int& reference;
    typedef int * pointer;
    typedef std::forward_iterator_tag iterator_category;
private:
    viter _orig;
};


//now try map
using miter = std::map<int, C>::iterator;
class iter2
{
public:
    typedef miter::difference_type difference_type;
    typedef miter::value_type value_type;
    typedef miter::reference reference;
    typedef miter::pointer * pointer;
    typedef miter::iterator_category iterator_category;

    iter2(miter v) : _orig(v)
    {
    }
    bool operator != (const iter2& z) const
    {
        return z._orig != _orig;
    }
    iter2& operator ++ ()
    {
        _orig++;
        return *this;
    }
    iter2 operator ++ (int)
    {
        ++_orig;
    }
    iter2 operator -- ();

    value_type operator * () const
    {
        return *_orig;
    }
    const value_type * operator ->() const
    {
        return &*_orig;
    }

  
private:
    miter _orig;
};

void xx()
{
    std::vector<C> foo = {5, 12, 56};
    const iter1 ii(foo.begin());
    const iter1 jj(foo.end());

    C x = *ii;
    int ww = ii->pp;
    
   assertEQ(std::distance(ii, jj), 3);

   std::map<int, C> baz;

   baz.insert( std::pair<int, C>(5, C(10)));
   const iter2 oo(baz.begin());
   const iter2 gg(baz.end());

   assertEQ(std::distance(oo, gg), 1);

   printf("ok\n");
}
