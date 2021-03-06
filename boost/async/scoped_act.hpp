#ifndef BOOST_ASYNC_SCOPED_ACT__HPP
#define BOOST_ASYNC_SCOPED_ACT__HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Based on the threader/joiner design from of Kevlin Henney (n1883)
//
// See http://www.boost.org/libs/async for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/async/algorithm/joinable.hpp>
#include <boost/async/algorithm/join.hpp>
#include <boost/async/fork.hpp>

#include <boost/config/abi_prefix.hpp>


namespace boost {
namespace async {

    template <typename ACT>
    class scoped_join {
        ACT& act_;
    public:
        scoped_join(ACT& act) : act_(act) {}
        ~scoped_join() {
            if (async::joinable(act_)) {
                async::join(act_);
            }
        }       
        ACT& get() {return act_;}; 
    };

    template <typename ACT>
    class scoped_terminate {
        ACT& act_;
    public:
        scoped_terminate(ACT& act) : act_(act) {}
        ~scoped_terminate() {
            if (async::joinable(act_)) {
                std::terminate();
            }
        }       
        ACT& get() {return act_;}; 
    };

    template <typename ACT>
    class scoped_fork_join {
        ACT act_;
    public:
        template <typename AE, typename F>
        scoped_fork_join(AE& ae, F f) 
            : act_(async::fork(ae, f))
        {}
        ~scoped_fork_join() {
            if (async::joinable(act_)) {
                async::join(act_);
            }
        }       
        ACT& get() {return act_;}; 
    };

    template <typename ACT>
    class scoped_fork_terminate {
        ACT act_;
    public:
        template <typename AE, typename F>
        scoped_fork_terminate(AE& ae, F f) 
            : act_(async::fork(ae, f))
        {}
       ~scoped_fork_terminate() {
            if (async::joinable(act_)) {
                std::terminate();
            }
        }       
        ACT& get() {return act_;}; 
    };

}
}


#include <boost/config/abi_suffix.hpp>

#endif

