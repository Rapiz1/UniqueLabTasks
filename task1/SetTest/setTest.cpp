#include<set>
#include<random>
#include<cstdio>
#include<ctime>
#include <assert.h>

#include"set.h"

#define Check(x, e) if(x) {printf(e);return;}

using uni::Set;
void Test(int cnt) {
    time_t start_time = time(NULL);
    std::set<int> stdset;
    Set<int> test_set;

    Check(stdset.empty() != test_set.empty(), "EmptyError\n");

    for(int i = 0; i < cnt; i++) {
        int k = rand() * (rand()>>1);
        stdset.insert(k);
        test_set.insert(k);
    }

    Check(stdset.size() != test_set.size(),"SizeError\n");
    int size = test_set.size();
    for(auto s : stdset) {
        Check(test_set.count(s) != 1, "CountError\n");
        test_set.erase(s);
        size--;
        Check(test_set.size() != size, "SizeError\n");
    }

    stdset.clear();
    test_set.clear();
    Check(test_set.size() != stdset.size(), "ClearError\n");
    
    time_t end_time = time(NULL);
    printf("Pass, takes %d time.\n", end_time - start_time);
    return;
}

void AdvTest(int cnt) {
    time_t start_time = time(NULL);
    std::set<int> stdset;
    Set<int> test_set;
    
    for(int lp = 0; lp < 2; lp++) {
        int minn = 0x7fffffff, maxx = 0;
        for(int i = 1;i < cnt; i++) {
            int k = rand() * (rand()>>1);
            minn = k > minn ? minn : k;
            maxx = k > maxx ? k : maxx;
            stdset.insert(k);
            test_set.insert(k);
        }

        //Find test
        std::set<int>::iterator stdit;
        Set<int>::iterator it;
        for(auto s : stdset) {
            it = test_set.find(s);
            Check(s != *it,"FindError\n");
        }
       puts("find pass");

        //upper lower test
        for(int i = 0; i < cnt; i++) {
            int k = rand();
            k = k % (maxx - minn + 1) + minn;
            //assert(k <= maxx && k >= minn);
            stdit = stdset.upper_bound(k);
            it = test_set.upper_bound(k);
            Check(*stdit != *it, "Upper_BoundError\n");
            stdit = stdset.lower_bound(k);
            it = test_set.lower_bound(k);
            Check(*stdit != *it, "Lower_BoundError\n");
        }
        puts("bound pass");

        //operator test
        for(int i = 0; i < 100; i++) {
            int k = rand() * (rand()>>1);
            k = k % (maxx - minn + 1) + minn;
            stdit = stdset.lower_bound(k);
            if (stdit == stdset.end())
                continue;
            it = test_set.lower_bound(k);
            int t = 0;
            while(*it != maxx && t < 100) {
                t++;
                stdit++;
                it++;
                Check(*stdit != *it, "++Error\n");
            }
            while(t--) {
                stdit--;
                it--;
                Check(*stdit != *it, "--Error\n");
            }
            Set<int>::iterator it2 = it;
            Check(*it != *it2, "=Error\n");
        }
        puts("op pass");

        stdset.clear();
        test_set.clear();
    }

    time_t end_time = time(NULL);
    printf("Pass, takes %d time.\n", end_time - start_time);
    return;
}

int main() {
    printf("randmax:%d\n", RAND_MAX);
    Test(100);
    Test(10000);
    Test(1000000);
    AdvTest(1000000);
    return 0;
}
