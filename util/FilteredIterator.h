#pragma once

#include <iterator>
template <typename T, typename UnderlyingIterator>
class filtered_iterator
{
public:
    typedef typename UnderlyingIterator::value_type value_type;
    typedef typename UnderlyingIterator::difference_type difference_type;
    typedef typename UnderlyingIterator::reference reference;
    typedef typename UnderlyingIterator::pointer * pointer;

    // we are not bidirectional, even if the underlying iterator is
    typedef std::forward_iterator_tag iterator_category;

    filtered_iterator(UnderlyingIterator i) : _it(i)
    {
    }
    bool operator != (const filtered_iterator& z) const
    {
        return z._it != this->_it;
    }
    filtered_iterator& operator ++ ()
    {
        _it++;
        return *this;
    }

    value_type operator * () const
    {
        return *_it;
    }
    const value_type * operator ->() const
    {
        return &*_it;
    }
private:
    UnderlyingIterator _it;
};

//now try map
#if 0
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
#endif

#if 0
void xx()
{
    std::vector<C> foo = {5, 12, 56};
    const iter1 ii(foo.begin());
    const iter1 jj(foo.end());

    C x = *ii;
    int ww = ii->pp;

    assertEQ(std::distance(ii, jj), 3);

    std::map<int, C> baz;

    baz.insert(std::pair<int, C>(5, C(10)));
    const iter2 oo(baz.begin());
    const iter2 gg(baz.end());

    assertEQ(std::distance(oo, gg), 1);

    printf("ok\n");
}
#endif


