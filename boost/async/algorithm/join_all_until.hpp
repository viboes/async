//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/async for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_ASYNC_JOIN_ALL_UNTIL__HPP
#define BOOST_ASYNC_JOIN_ALL_UNTIL__HPP

#include <boost/fusion/include/transform.hpp>

#ifdef BOOST_HAS_CHRONO_LIB
#include <boost/chrono/chrono.hpp>
#else
#include <boost/thread/thread_time.hpp>
#endif
#include <boost/async/algorithm/join_until.hpp>

#include <boost/config/abi_prefix.hpp>

namespace boost {
namespace async {

    namespace fct {
        struct join_until {
            join_until(const system_time& abs_time) : abs_time_(abs_time) {}
            template<typename ACT>
            bool operator()(ACT& act) const {
                return async::join_until(act, abs_time_);
            }
        private:
            const system_time& abs_time_;
        };
    }

    namespace fct {
        struct join_for {
            template <typename Duration>
            join_for(const Duration& rel_time) : abs_time_(get_system_time()+rel_time) {}
            template<typename ACT>
            bool operator()(ACT& act) const {
                return join_until(act, abs_time_);
            }
        private:
            const system_time& abs_time_;
        };
    }

    namespace result_of {
        template <typename Sequence>
        struct join_all_until {
            typedef bool type;
        };
    }
    namespace result_of {
        template <typename Sequence>
        struct join_all_for {
            typedef bool type;
        };
    }

namespace detail {
        template<int N>
    struct join_all_until_unrolled
    {
        template<typename I0>
        static bool call(I0 const& i0, const system_time& abs_time)
        {
            if (fct::join_until(abs_time)(*i0)) return true;
            else {
                typedef typename fusion::result_of::next<I0>::type I1;
                I1 i1(fusion::next(i0));
                if(fct::join_until(abs_time)(*i1)) return true;
                else {
                    typedef typename fusion::result_of::next<I1>::type I2;
                    I2 i2(fusion::next(i1));
                    if(fct::join_until(abs_time)(*i2)) return true;
                    else {
                        typedef typename fusion::result_of::next<I2>::type I3;
                        I3 i3(fusion::next(i2));
                        if(fct::join_until(abs_time)(*i3)) return true;
                        else {
                            return join_all_until_unrolled<N-4>::call(fusion::next(i3), abs_time);
                        }
                    }
                }
            }
        }
    };

    template<>
    struct join_all_until_unrolled<3>
    {
        template<typename I0>
        static bool call(I0 const& i0, const system_time& abs_time)
        {
            if (fct::join_until(abs_time)(*i0)) return true;
            else {
                typedef typename fusion::result_of::next<I0>::type I1;
                I1 i1(fusion::next(i0));
                if(fct::join_until(abs_time)(*i1)) return true;
                else {
                    typedef typename fusion::result_of::next<I1>::type I2;
                    I2 i2(fusion::next(i1));
                    return fct::join_until(abs_time)(*i2);
                }
            }
        }
    };

    template<>
    struct join_all_until_unrolled<2>
    {
        template<typename I0>
        static bool call(I0 const& i0, const system_time& abs_time)
        {
            if (fct::join_until(abs_time)(*i0)) return true;
            else {
                typedef typename fusion::result_of::next<I0>::type I1;
                I1 i1(fusion::next(i0));
                return fct::join_until(abs_time)(*i1);
            }
        }
    };

    template<>
    struct join_all_until_unrolled<1>
    {
        template<typename I0>
        static bool call(I0 const& i0, const system_time& abs_time)
        {
            return fct::join_until(abs_time)(*i0);
        }
    };

    template<>
    struct join_all_until_unrolled<0>
    {
        template<typename It>
        static bool call(It const&, const system_time&)
        {
            return true;
        }
    };

    template <typename Sequence>
    inline typename result_of::template join_all_until<Sequence>::type
    join_all_until(Sequence& seq, const system_time& abs_time, fusion::random_access_traversal_tag)
    {
        typedef typename fusion::result_of::begin<Sequence>::type begin;
        typedef typename fusion::result_of::end<Sequence>::type end;
        return join_all_until_unrolled<fusion::result_of::distance<begin, end>::type::value>::call(fusion::begin(seq), abs_time);
    }


}


    template <typename Sequence>
    typename result_of::template join_all_until<Sequence>::type
    join_all_until(Sequence& seq, const system_time& abs_time) {
#if 1
        return detail::join_all_until(seq, abs_time, typename fusion::traits::category_of<Sequence>::type());
#else
        bool r = fct::join_until(abs_time)(fusion::at_c<0>(t));
        if (r) return true;
        else {
            if (fusion::size(t)==1) {
                return false;
            } else {
                return join_all_until(fusion::pop_front(t), abs_time);
            }
        }
#endif
    }

    template <typename Sequence, typename Duration>
    typename result_of::template join_all_for<Sequence>::type
    join_all_for(Sequence& seq, Duration rel_time) {
        return join_all_until(seq, get_system_time()+rel_time);
    }

}
}   // namespace boost

#include <boost/config/abi_suffix.hpp>

#endif
