#include<queue>
#include<random>
#include<cstdio>
#include<ctime>

#include"priority_queue.h"

#define Check(x, e) if(x) {printf(e);return;}
using uni::PriorityQueue;
void Test(int cnt) {
    time_t start_time = time(NULL);
    std::priority_queue<int> stdqueue;
    PriorityQueue<int> test_queue;
    Check(stdqueue.empty() != test_queue.empty(), "EmptyError\n");
    for(int i = 0; i < cnt; i++) {
        int k = rand()%1000;
        stdqueue.push(k);
        test_queue.push(k);
    }
    Check(stdqueue.size() != test_queue.size(), "SizeError\n");
    for(int i = 0; i < cnt; i++) {
        int k = stdqueue.top();
        Check(test_queue.top() != k, "TopError\n");
        stdqueue.pop();
        test_queue.pop();
        Check(stdqueue.size() != test_queue.size(), "SizeError\n");
    }
    Check(stdqueue.empty() != test_queue.empty(), "EmptyError\n");
    time_t end_time = time(NULL);
    printf("Pass, takes %d time.\n", int(end_time - start_time));
    //std::cout << (end_time - start_time) << ", pass\n";
    return;
}

int main() {
    Test(100);
    Test(10000);
    Test(1000000);
    return 0;
}
