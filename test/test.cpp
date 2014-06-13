#include <pythonic.hpp>
#include <cstdio>
#include <vector>
#include <list>
#include <cmath>
#include <map>
#include <functional>

using namespace std;
using namespace pythonic;

int main(int argc, char *argv[]) {

    // do a prime number computation

    int N = 1000;
    auto r = range(2, sqrt(N));
    auto bv = vector<int>(N);
    bv[0] = bv[1] = 1;

    enumerate(bv).filter([=](pair<size_t, int> p) {return !p.second && (p.first < sqrt(N));}).foreach([&bv,N](pair<size_t, int> p) {
            int k = p.first;
            foreach(bv, [](int &x) {x = 1;}, bv.begin()+k*k, bv.end(), [&bv,k](vector<int>::iterator it) {
                return (it > bv.end() - k)?bv.end():(it+k);
            });
    });

    auto l = enumerate(bv).filter([](pair<size_t, int> bp) {return !bp.second;}).len();

    printf("total of %ld primes < %d\n", l, N);

    // create a count histogram out of a vector
    vector<int> v({1,2,3,1,7,2,3,0,1});
    map<int, int> histogram;
    foreach(v, [&](int x) { histogram[x]++; });

    foreach(histogram, [](pair<int, int> histpair) { printf("count[%d] = %d\n", histpair.first, histpair.second); });

    // find the maximum odd value in a vector
    auto m = filter(v, [](int x) { return x % 2 != 0; }).max();
    printf("maximum odd value = %d\n", m);

    // find the maximum occuring value in a vector
    map<int, int> histogram2;

    foreach(v, [&](int x) { histogram2[x]++; });

    // here we have to use the intermediate transform(...) because a map iterator upon dereference yields a pair<const int, int> which can not be repeatedly assigned in a loop :(
    auto m2 = transform(histogram2, [](pair<const int, int> p) {return pair<int, int>(p.first, p.second);}).max([](pair<int, int> p) {return p.second;}).first;
    printf("mode(v) = %d\n", m2);

}
