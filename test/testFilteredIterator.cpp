#include <assert.h>
#include <vector>

#include "asserts.h"
#include "FilteredIterator.h"


void test0()
{
    std::vector<int> v = {5, 12, 56};
    filtered_iterator<int, std::vector<int>::iterator> it_beg(v.begin());
    filtered_iterator<int, std::vector<int>::iterator> it_end(v.end());

    assert(it_beg != it_end);
    assert(*it_beg == 5);

    assert(*(v.begin()) == 5);

  // assertEQ(std::distance(it_beg, it_end), 3);
   ++it_beg;
   assert(*it_beg == 12);



#if 0
    std::map<int, C> baz;

    baz.insert(std::pair<int, C>(5, C(10)));
    const iter2 oo(baz.begin());
    const iter2 gg(baz.end());

    assertEQ(std::distance(oo, gg), 1);

    printf("ok\n");
#endif

}

class C
{
public:
    int pp;
    C(int q) : pp(q)
    {
    }
};


void test1()
{
    std::vector<C> v = {5, 12, 56};
    filtered_iterator<C, std::vector<C>::iterator> it_beg(v.begin());

    filtered_iterator<C, std::vector<C>::iterator> it_end(v.end());

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