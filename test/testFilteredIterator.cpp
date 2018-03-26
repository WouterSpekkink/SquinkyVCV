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


void testx()
{
#if 0
    std::vector<C> v = {5, 12, 56};

    using iter_t = filtered_iterator<C, std::vector<C>::iterator>;
    iter_t::filter_func is_even = [](const C& a) {
        return !(a.pp % 2);
    };
    iter_t it_beg(v.begin(), is_even);

    iter_t it_end(v.end(), is_even);

    assert(it_beg != it_end);
    assertEQ(std::distance(it_beg, it_end), 3);

    assert((*it_beg).pp == 5);

    assert(it_beg->pp == 5);
    
    int count = 0;
    for (; it_beg != it_end; ++it_beg) {
        printf("one more %d\n", it_beg->pp);
        ++count;
    }
    assertEQ(count, 3);

#endif
#if 0
    // assertEQ(std::distance(it_beg, it_end), 3);
    ++it_beg;
    assert(*it_beg == 12);
#endif
}



void testFilteredIterator()
{
    test0();
    test1();
}