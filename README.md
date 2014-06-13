pythonic++
==========

pythonic++: A library for adding python-like syntactic sugar to C++11.

This header file tries to create some simple utility functions that are inspired by Python inbuilt functions such as `len`, `min`, etc. A brief summary of all the functionality in the package is below

The base `iterable<T,R>` type is at the core of the provided functionality. This class is the base of a generic iterable that supports forward iteration only, and exopses an iterator that is either of `ITERATOR_END` type, or `ITERATOR_NORMAL` type. Note that we treat any two `END` iterators as equal, while any other pair of iterators as non-equal. This is because in a generic iterable there is no way to ensure that two iterators are pointing to the exact same iteration point. Thus, the only type of iteration supported by any `iterable` is of the type

```C++
for(auto it = itr.begin(); it != itr.end(); ++it) {...}
```

OR

```C++
for(auto v : itr) {...}
```

Any iterable supports the following methods:
* `filter(Function f)`: returns another `iterable` that only contains elements `x` for which `f(x) == true`.
* `transform(Function f)`: returns another `iterable` with the values `f(x)` for each `x` in the `iterable`.
* `enumerate()`: returns another `iterable` that contains a `pair<size_t, T>(i,x)` where `i` is the iteration index for each `x`.
* `max()`: returns the maximum value `x` seen in this iterable.
* `max(Function f)`: returns the value `x` such that `f(x)` is maximum.
* `min()`: returns the minimum value `x` seen in this iterable.
* `min(Function f)`: returns the value `x` such that `f(x)` is minimum.
* `len()`: returns the length of this iterable.
* `foreach(Function f)`: returns the original `iterable` while performing the computation `f(x)` for each `x`. Note that this does not a changed `iterable`. For changing, see the function `transform`.
* `to_vector()`: returns a `vector<T>` that contains each value from this `iterable` in sequence.
* `(Pushable)`: casts the iterable to the type `Pushable` that supports a `push_back` operation.

For STL containers that are not derived from the `iterable` class but support iteration by `begin()`, `end()`, and `++`, the library defines convenience functions similar to the above. Specifically each `iterable::member_function(...)` has a counterpart `member_function(v, ...)` which can operate on any such STL container. Two more functions are defined on STL containers:
* `foreach(V &v, Function fun, IterFunction ifun)`: This is equivalent to `for(auto it = v.begin(); it != v.end(); it = iterfun(it)) { fun(*it); }`
* `foreach(V &v, Function fun, V::iterator beginit, V::iterator endit, IterFunction ifun)`: This is equivalent to `for(auto it = beginit; it != endit; it = iterfun(it)) { fun(*it); }`

Also, there is a function `range()` which iterates over all integers in a given range. The variations are:
* `range(N)`: iterates through `0, 1, ..., N-1`
* `range(M,N)`: iterates through `M, M+1, ..., N-1`
* `range(M,N,step)`: iterates through `M, M+step, M+2*step, ..., N-1`

Some simple examples that illustrate the functionality of the library is in the tests/test.cpp file.
