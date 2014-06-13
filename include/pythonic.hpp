#ifndef _PYTHONIC_HPP_
#define _PYTHONIC_HPP_
#include <cassert>
#include <inttypes.h>
#include <type_traits>
#include <tuple>
#include <cstdio>
#include <utility>
#include <iostream>
#include <vector>
#include <algorithm>

namespace pythonic {

enum iterator_type_t {
    ITERATOR_NORMAL,
    ITERATOR_END
};

template<typename T, typename R> struct iterable;
template<typename T, typename R, class Function> struct filtered_;
template<typename T, typename R, class Function> struct transformer_;
template<typename T> struct range_;
template<typename T> struct enumerator_;
struct min_;
struct max_;

template<class V>
struct stl_type {
    typedef typename std::remove_reference<decltype(*(std::declval<V>().begin()))>::type _Tp;
};

template <typename T>
class has_size
{
    typedef char one;
    typedef long two;

    template <typename C> static one test( decltype(typename std::remove_reference<C>::type().size()) ) ;
    template <typename C> static two test(...);


    public:
    enum { value = (sizeof(test<T>(0)) == sizeof(char)) };
};

struct len_ {
    template<typename T>
    typename std::enable_if<!(has_size<T>::value), size_t>::type operator() (T&& x) {
        size_t i = 0;
        for(auto t : x) {
            i++;
        }
        return i;
    }

    template<typename T>
    typename std::enable_if<has_size<T>::value, size_t>::type operator() (T&& x) {
        return x.size();
    }

};

template<typename T>
size_t len(T&& x) {
    return len_()(x);
}

struct min_ {
    template<typename Iterable>
    auto operator() (Iterable&& x) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
        typename std::remove_const<typename std::remove_reference<decltype(*(x.begin()))>::type>::type minVal = *(x.begin());

        for(auto y : x) {
            if(y < minVal) {
                minVal = y;
            }
        }

        return minVal;
    }

    template<typename Iterable, class Function>
    auto operator() (Iterable&& x, Function f) -> typename std::remove_reference<decltype(*(x.begin()))>::type {
        typename std::remove_const<typename std::remove_reference<decltype(*(x.begin()))>::type>::type minVal = *(x.begin());

        for(auto y : x) {
            if(f(y) < f(minVal)) {
                minVal = y;
            }
        }

        return minVal;
    }
};

template<typename Iterable>
auto min(Iterable&& x) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
    return min_()(x);
}

template<typename Iterable, class Function>
auto min(Iterable&& x, Function f) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
    return min_()(x, f);
}

struct max_ {
    template<typename Iterable>
    auto operator() (Iterable&& x) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
        typename std::remove_const<typename std::remove_reference<decltype(*(x.begin()))>::type>::type maxVal = *(x.begin());

        for(auto y : x) {
            if(y > maxVal) {
                maxVal = y;
            }
        }

        return maxVal;
    }

    template<typename Iterable, class Function>
    auto operator() (Iterable&& x, Function f) -> typename std::remove_reference<decltype(*(x.begin()))>::type {
        typename std::remove_const<typename std::remove_reference<decltype(*(x.begin()))>::type>::type maxVal = *(x.begin());

        for(auto y : x) {
            if(f(y) > f(maxVal)) {
                maxVal = y;
            }
        }

        return maxVal;
    }
};

template<typename Iterable>
auto max(Iterable&& x) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
    return max_()(x);
}

template<typename Iterable, class Function>
auto max(Iterable&& x, Function f) -> typename std::remove_reference<decltype(*(x.begin()))>::type const {
    return max_()(x, f);
}

// generic iterable object, can copy itself to a pushable object
template<typename T, typename R>
struct iterable {

    // base iterator class
    // has to define a custom END iterator so that filtering can
    // work properly
    struct iterator {
        R &parent;
        T val;
        iterator_type_t type;

        bool operator==(const iterator &other) const {
            if(type == ITERATOR_END && other.type == ITERATOR_END)
                return true;
            return false;
        }

        bool operator!=(const iterator &other) const {
            return !(operator==(other));
        }

        T operator*() {
            return val;
        }

        iterator(R &_parent, iterator_type_t _type) :
            parent(_parent), type(_type) {}

    };

    template<class Pushable>
    operator Pushable() {
        Pushable p;
        R &r = *(static_cast<R*>(this));
        for(T x : r) {
            p.push_back(x);
        }
        return p;
    }

    std::vector<T> to_vector() {
        std::vector<T> v = *this;
        return v;
    }

    template<class Function>
    filtered_<T, R, Function> filter(Function f) {
        R &r = *(static_cast<R*>(this));
        return filtered_<T, R, Function>(std::forward<R>(r), f);
    }

    template<class Function>
    auto transform(Function f) -> transformer_<R, decltype(f(T())), Function> {
        R &r = *(static_cast<R*>(this));
        return transformer_<R, decltype(f(T())), Function>(std::forward<R>(r), f);
    }

    enumerator_<R> enumerate() {
        R &r = *(static_cast<R*>(this));
        return enumerator_<R>(std::forward<R>(r));
    }

    T min() {
        R &r = *(static_cast<R*>(this));
        return min_()(std::forward<R>(r));
    }

    template<class Function>
    T min(Function f) {
        R &r = *(static_cast<R*>(this));
        return min_()(std::forward<R>(r), f);
    }

    T max() {
        R &r = *(static_cast<R*>(this));
        return max_()(std::forward<R>(r));
    }

    template<class Function>
    T max(Function f) {
        R &r = *(static_cast<R*>(this));
        return max_()(std::forward<R>(r), f);
    }

    size_t len() {
        R &r = *(static_cast<R*>(this));
        return len_()(std::forward<R>(r));
    }

    template<class Function>
    R& foreach(Function f) {
        R &r = *(static_cast<R*>(this));
        for(auto x : r) {
            f(x);
        }
        return r;
    }

};

template<typename T, class Function>
T& foreach(T& x, Function f) {
    for(auto v : x) {
        f(v);
    }
    return x;
}

template<typename T, class Function, class IterFunction>
T& foreach(T& x, Function fun, IterFunction ifun) {
    for(auto it = x.begin(); it != x.end(); it = ifun(it)) {
        fun(*it);
    }
    return x;
}

template<typename T, class Function, class IterFunction>
void foreach(T& x, Function fun, typename T::iterator iter_begin, typename T::iterator iter_end, IterFunction ifun) {
    for(auto it = iter_begin; it < iter_end; it = ifun(it)) {
        fun(*it);
    }
}

// sorted, returns a vector
template<typename V>
std::vector<typename stl_type<V>::_Tp> sorted(const V &v) {
    std::vector<typename stl_type<V>::_Tp> vv(v.begin(), v.end());
    std::sort(vv.begin(), vv.end());
    return vv;
}

template<typename T, typename R, class Function>
struct filtered_ : public iterable<T, filtered_<T, R, Function>> {
    R r;
    Function f;

    typedef iterable<T, filtered_<T, R, Function>> base;
    typedef typename base::iterator base_itr;
    typedef typename std::remove_reference<R>::type::iterator itr_type;

    filtered_(R &&_r, Function _f) :
        r(std::forward<R>(_r)), f(_f) {}

    struct iterator : public base_itr {
        itr_type unfiltered_itr;

        iterator &operator++() {
            consume_until_hit();
            return *this;
        }

        void consume_until_hit() {
            if(unfiltered_itr == base_itr::parent.r.end()) {
                base_itr::type = ITERATOR_END;
                return;
            }

            do {
                ++unfiltered_itr;
            } while((!base_itr::parent.f(*unfiltered_itr)) &&
                    base_itr::parent.r.end() != unfiltered_itr);

            if(unfiltered_itr == base_itr::parent.r.end()) {
                base_itr::type = ITERATOR_END;
                return;
            }

            base_itr::val = *unfiltered_itr;
            base_itr::type = ITERATOR_NORMAL;
        }

        iterator(filtered_<T,R,Function> &_parent, itr_type _unfiltered) :
            base_itr(_parent, ITERATOR_NORMAL),
            unfiltered_itr(_unfiltered) {
                if(unfiltered_itr == base_itr::parent.r.end()) {
                    base_itr::type = ITERATOR_END;
                    return;
                }

                while((!base_itr::parent.f(*unfiltered_itr)) &&
                        base_itr::parent.r.end() != unfiltered_itr) {
                    ++unfiltered_itr;
                } 

                if(unfiltered_itr == base_itr::parent.r.end()) {
                    base_itr::type = ITERATOR_END;
                    return;
                }

                base_itr::val = *unfiltered_itr;
                base_itr::type = ITERATOR_NORMAL;
            }

    };

    iterator begin() {
        return iterator(*this, r.begin());
    }

    iterator end() {
        return iterator(*this, r.end());
    }
};

// class for extending iterator interface to inbuilt classes
// implementation of filter() for inbuilt classes without filter
template<class __Iterable, class Function>
auto filter(__Iterable &&v, Function f)
    -> filtered_<typename stl_type<__Iterable>::_Tp, __Iterable, Function> {
    return filtered_<typename stl_type<__Iterable>::_Tp, __Iterable, Function>(std::forward<__Iterable>(v), f);
}

// implementation of range(...) as an iterator
template<typename T>
struct range_ : public iterable<T, range_<T>> {
    T step;
    T start;
    T finish;

    typedef iterable<T, range_<T>> base;
    typedef typename base::iterator base_itr;

    struct iterator : public base_itr {

        iterator &operator++() {
            base_itr::val += base_itr::parent.step;
            if(base_itr::val == base_itr::parent.finish)
                base_itr::type = ITERATOR_END;
            return *this;
        }

        iterator(range_<T> &_parent, T &_val, iterator_type_t _type) :
            base_itr(_parent, _type) {
                base_itr::val = _val;
                if(base_itr::val == base_itr::parent.finish)
                    base_itr::type = ITERATOR_END;
            }
    };

    // generic construct with range checks and proper end initialization
    void construct(T _start, T _finish, T _step) {
        assert( _step != 0 && "step must not be zero");
        assert( (_finish - _start) * _step >= 0 && "finish must be reachable from start by step");
        start = _start;
        step = _step;
        T _numsteps = (_finish - _start)/_step;
        finish = ((_numsteps * _step + _start) == _finish)?_finish:(_numsteps * _step + _start + _step);
    }

    // can't instantiate without any arguments
    range_() =delete;

    // range(789)
    range_(T _finish) {
        construct(0, _finish, 1);
    }

    range_(T _start, T _finish) {
        construct(_start, _finish, 1);
    }

    range_(T _start, T _finish, T _step) {
        construct(_start, _finish, _step);
    }

    iterator begin() {
        return iterator(*this, start, ITERATOR_NORMAL);
    }

    iterator end() {
        return iterator(*this, finish, ITERATOR_END);
    }

};

typedef range_<int64_t> range;

template <class T>
struct enumerator_ : public iterable<std::pair<size_t, typename stl_type<T>::_Tp>, enumerator_<T>>{
    T v;
    enumerator_(T&& _v) : v(std::forward<T>(_v)) {}

    typedef typename std::remove_reference<T>::type::iterator itr_type;
    typedef typename std::pair<size_t, typename stl_type<T>::_Tp> pair_type;
    typedef iterable<pair_type, enumerator_<T>> base;
    typedef typename base::iterator base_itr;

    struct iterator : public base_itr {
        size_t i;
        itr_type v_itr;

        iterator &operator++() {
            if(v_itr == base_itr::parent.v.end()) {
                base_itr::type = ITERATOR_END;
            }

            ++v_itr;
            ++i;

            if(v_itr == base_itr::parent.v.end()) {
                base_itr::type = ITERATOR_END;
            }

            base_itr::val = pair_type(i, *v_itr);
            return *this;
        }

        iterator(enumerator_<T> &_parent, itr_type _v_itr, size_t _i) :
            base_itr(_parent, ITERATOR_NORMAL), v_itr(_v_itr), i(_i) {
                if(v_itr == base_itr::parent.v.end())
                    base_itr::type = ITERATOR_END;
                else {
                    base_itr::val = pair_type(i, *v_itr);
                }
            }
    };

    iterator begin() {
        return iterator(*this, v.begin(), 0);
    }

    iterator end() {
        return iterator(*this, v.end(), -1);
    }

};

template <class T>
enumerator_<T> enumerate(T&& x) {
    return enumerator_<T>(std::forward<T>(x));
}

template <class T, class R, class Function>
struct transformer_ : public iterable<R, transformer_<T, R, Function>>{
    T v;
    Function f;
    transformer_(T&& _v, Function _f) : v(std::forward<T>(_v)), f(_f) {}

    typedef typename std::remove_reference<T>::type::iterator itr_type;
    typedef iterable<R, transformer_<T, R, Function>> base;
    typedef typename base::iterator base_itr;

    struct iterator : public base_itr {
        itr_type v_itr;

        iterator &operator++() {
            if(v_itr == base_itr::parent.v.end()) {
                base_itr::type = ITERATOR_END;
            }

            ++v_itr;

            if(v_itr == base_itr::parent.v.end()) {
                base_itr::type = ITERATOR_END;
            }

            base_itr::val = base_itr::parent.f(*v_itr);
            return *this;
        }

        iterator(transformer_<T, R, Function> &_parent, itr_type _v_itr) :
            base_itr(_parent, ITERATOR_NORMAL), v_itr(_v_itr) {
                if(v_itr == base_itr::parent.v.end())
                    base_itr::type = ITERATOR_END;
                else {
                    base_itr::val = base_itr::parent.f(*v_itr);
                }
            }
    };

    iterator begin() {
        return iterator(*this, v.begin());
    }

    iterator end() {
        return iterator(*this, v.end());
    }

};

template <class T, class Function>
auto transform(T&& x, Function f) -> transformer_<T, decltype(f(*(x.begin()))), Function> {
    return transformer_<T, decltype(f(*(x.begin()))), Function>(std::forward<T>(x), f);
}


};

#endif
