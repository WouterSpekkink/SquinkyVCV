#include <assert.h>
#include <vector>

#include "asserts.h"
#include "FilteredIterator.h"


void test0()
{
    std::vector<int> v = {6, 11, 56};
    using iter_t = filtered_iterator<int, std::vector<int>::iterator>;

    iter_t::filter_func is_even = [](int a) {
        return !(a % 2);
    };

    iter_t  it_beg(v.begin(), v.end(), is_even);
    iter_t  it_end(v.end(), v.end(), is_even);

    assertEQ(std::distance(it_beg, it_end), 2);

    assertEQ(*it_beg, 6);
    ++it_beg;
    assertEQ(*it_beg, 56);
    ++it_beg;
    assert(it_beg == it_end);
}

void test1()
{
    std::vector<int> v = {11, 56};
    using iter_t = filtered_iterator<int, std::vector<int>::iterator>;

    iter_t::filter_func is_even = [](int a) {
        return !(a % 2);
    };

    iter_t  it_beg(v.begin(), v.end(), is_even);
    iter_t  it_end(v.end(), v.end(), is_even);

   // assertEQ(std::distance(it_beg, it_end), 1);

    assertEQ(*it_beg, 56);
    ++it_beg;
    assert(it_beg == it_end);
}

class C
{
public:
    int pp;
    C(int q) : pp(q)
    {
    }
};

static void test2()
{
    using iterator = std::vector<C>::iterator;
    using iter_f = filtered_iterator<C, std::vector<C>::iterator>;
    std::vector<C> x = {10, 20, 30};

    iterator i = x.begin();
    iter_f f(x.begin(), x.end(), [](const C&) { return true; });

    assertEQ(f->pp, i->pp);
    assertEQ((++f)->pp, (++i)->pp);
    assertEQ((f++)->pp, (i++)->pp);

}



void testFilteredIterator()
{
    test0();
    test1();
    test2();
}