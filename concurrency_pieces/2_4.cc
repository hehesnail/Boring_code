#include <iostream>
#include <numeric>
#include <thread>
#include <algorithm>
#include <vector>
#include <functional>

using namespace std;

template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    
    unsigned long const length = distance(first, last);
    
    if (!length)
        return init;
    
    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    unsigned long const hardware_threads = thread::hardware_concurrency();
    unsigned long const num_threads = min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    vector<T> results(num_threads);
    vector<thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); i++) {
        Iterator block_end = block_start;
        advance(block_end, block_size);
        threads[i] = thread(accumulate_block<Iterator, T>(), 
                            block_start, block_end, ref(results[i]));
        block_start = block_end;
    }

    accumulate_block<Iterator, T>(block_start, end, results[num_threads - 1]);
    for_each(threads.begin(), threads.end(), mem_fn(&thread::join));

    return accumulate(results.begin(), results.end(), init);

}

int main() {

    vector<int> v(100000);

    for (int i = 0; i < v.size(); i++)
        v[i] = i + 1;

    cout << parallel_accumulate(v.begin(), v.end(), 0) << endl;

    return 0;
}