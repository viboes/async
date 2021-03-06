//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2008-2009.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/async for documentation.
//
// Based on the shared.cpp example from the threadalert library of Roland Schwarz
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>

#include <boost/progress.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/tp/pool.hpp>
#include <boost/tp/unbounded_channel.hpp>
#include <boost/tp/fifo.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/algorithm/inplace_merge.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/async/fork.hpp>
#include <boost/async/algorithm/wait.hpp>
#include <boost/async/scheduler.hpp>
#include <boost/array.hpp>

#include <assert.h>

#include <boost/range/algorithm/count.hpp>

#if 0

struct A {
    int x;
    int y;
};

namespace boost {
    //template <typename T>
    unsigned count(A& t, const int& i) {
        return 2;
    }
}


unsigned xx() {
    A a;
    return boost::count(a, 1);
}

int main() {
    std::cout << xx() << std::endl; 
        
}

#else

#define BOOST_NB_OF_THREADS 4
#define BOOST_PARTS 2
#define NN 400000

class scoped_timer {
    boost::posix_time::ptime start_;
public:    
    scoped_timer() 
        : start_(boost::posix_time::microsec_clock::universal_time())
    {}
    ~scoped_timer() {
        boost::posix_time::ptime stop( boost::posix_time::microsec_clock::universal_time() );
        std::cout << " " << ( stop - start_).total_milliseconds() << " milli seconds" << std::endl;
    }
};

template <typename Range>
class partition
{
public:
    boost::iterator_range<typename boost::range_iterator<Range>::type> range_;
    std::size_t parts_;
    partition(boost::iterator_range<typename boost::range_iterator<Range>::type>& range, std::size_t parts):
        range_(range),
        parts_(parts)
    {}
    boost::iterator_range<typename boost::range_iterator<Range>::type> operator[](unsigned i) {
        unsigned size = boost::size(range_);
        if (i<(parts_-1))
            return boost::sliced_range<typename boost::range_iterator<Range>::type>(range_, i*(size/parts_), ((i+1)*(size/parts_)));
        else 
            return boost::sliced_range<typename boost::range_iterator<Range>::type>(range_, (parts_-1)*(size/parts_), size);
    }
};

typedef boost::tp::pool<
  boost::tp::unbounded_channel< boost::tp::fifo >
> pool_type;

#ifdef TASK_POOL
        typedef boost::tp::task< pool_type,  void > task_type;
#else
        typedef boost::tp::task< void > task_type;
#endif


template <
    typename DirectSolver,
    typename Composer,
    typename AE,
    typename Range
>
  void inplace_solve( AE & ae, 
        boost::iterator_range<typename boost::range_iterator<Range>::type> range, 
        unsigned cutoff );
        
template <
    typename DirectSolver,
    typename Composer,
    typename AE,
    typename Range
>
  void inplace_solve( AE & ae, 
        boost::iterator_range<typename boost::range_iterator<Range>::type> range, 
        unsigned cutoff )
  {
    unsigned size = boost::size(range);
    //std::cout << "<<par_ " << size;  
    if ( size <= cutoff) DirectSolver()(range);
    else {
        partition<Range> parts(range, BOOST_PARTS);
        std::list<task_type> tasks;
        #if 0 // this code do not compiles with gcc 3.4.4 cygwin
        boost::transform(parts, boost::begin(tasks), 
                          boost::bind(&AE::submit, boost::ref(ae),
                          //boost::bind(&boost::async::fork<AE>, boost::ref(ae),
                                      boost::bind(&inplace_solve<DirectSolver,Composer,AE,Range>, boost::ref(ae),_1,cutoff)));
        #else
        for (unsigned i=0;i < BOOST_PARTS-1; ++i) {
            task_type tmp(ae.submit(
                boost::bind(
                    &inplace_solve<DirectSolver,Composer,AE,Range>,
                    boost::ref(ae),
                    parts[i],
                    cutoff
            )));
            tasks.push_back(tmp);
        }
        #endif
        inplace_solve<DirectSolver,Composer,AE,Range>(ae, parts[BOOST_PARTS-1], cutoff);
        boost::for_each(tasks, &boost::async::wait_act<task_type>);
        
        //std::cout << "par_inplace_merge_fct " << size << ">>"<< std::endl;  
        #if BOOST_PARTS == 4
        partition<Range> sorted(range, BOOST_PARTS/2);
        Composer composer;
        for (unsigned i=0;i < BOOST_PARTS-1; ++i) {
            task_type tmp(ae.submit(
                boost::bind(
                    composer,
                    parts[i]
            )));
            tasks.push_back(tmp);
        }
        boost::for_each(tasks, &boost::async::wait_act<task_type>);
        #endif
        Composer()(range);

        //std::cout << "par_ " << size << ">>"<< std::endl;  
        
    }
  }

struct sort_fct {
    template<class RandomAccessRange>
    RandomAccessRange& operator()(RandomAccessRange rng) {
        return boost::sort(rng);
    }
};

struct inplace_merge_fct {
    template<class BidirectionalRange>
    BidirectionalRange&
    operator()( BidirectionalRange rng) {
        return boost::inplace_merge(rng, boost::begin(rng)+(boost::size(rng)/2));
    }
};
template <typename AE, typename Range>
void parallel_sort(AE& ae, Range& range, unsigned cutoff=10000) {
    boost::iterator_range<typename boost::range_iterator<Range>::type> rng(range);
    inplace_solve<sort_fct,inplace_merge_fct,pool_type,Range>( ae, rng, cutoff);
}

int sorted[NN];
int values1[NN];
int values2[NN];
int values3[NN];
int values4[NN];
int values5[NN];
int values6[NN];

int main() {
    for (unsigned i=0; i<NN; ++i) sorted[i]=i;
   
    for (unsigned i=0; i<NN; ++i) values1[i]=NN-i-1;
    {
    std::cout << "std::sort: reverse 0.." << NN;
    scoped_timer t;  // start timing
    std::sort(boost::begin(values1), boost::end(values1));
    }
    assert(boost::equal(values1, sorted));
    {
    std::cout << "std::sort: 0.." << NN;
    scoped_timer t;  // start timing
    std::sort(boost::begin(values1), boost::end(values1));
    }
    
    for (unsigned i=0; i<NN; ++i) values2[i]=NN-i-1;
    {
    std::cout << "boost::sort: reverse 0.."<<NN;
    scoped_timer t;  // start timing
    boost::sort(values2);
    }
    assert(boost::equal(values2, sorted));
    {
    std::cout << "boost::sort: 0.."<<NN;
    scoped_timer t;  // start timing
    boost::sort(values2);
    }

    // creates a threadpool with two worker-threads
    pool_type pool( boost::tp::poolsize(BOOST_NB_OF_THREADS) );

    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    {
    std::cout << "parallel_sort "<<NN/2<<":  reverse 0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/2);
    }   
    assert(boost::equal(values5, sorted));
    {
    std::cout << "parallel_sort "<<NN/2<<":  0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/2);
    }   

    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    {
    std::cout << "parallel_sort "<<NN/4<<":  reverse 0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/4);
    }   
    assert(boost::equal(values5, sorted));
    {
    std::cout << "parallel_sort "<<NN/4<<":  0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/4);
    }   

    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    {
    std::cout << "parallel_sort "<<NN/8<<":  reverse 0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/8);
    }   
    assert(boost::equal(values5, sorted));
    {
    std::cout << "parallel_sort "<<NN/8<<":  0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/8);
    }   

    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    {
    std::cout << "parallel_sort "<<NN/16<<":  reverse 0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/16);
    }   
    assert(boost::equal(values5, sorted));
    {
    std::cout << "parallel_sort "<<NN/16<<":  0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/16);
    }   

#if 0
    for (unsigned i=0; i<NN; ++i) values5[i]=NN-i-1;
    {
    std::cout << "parallel_sort "<<NN/32<<":  reverse 0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/32);
    }   
    assert(boost::equal(values5, sorted));
    {
    std::cout << "parallel_sort "<<NN/32<<":  0.."<<NN;
    scoped_timer tmr;  // start timing
    parallel_sort(pool, values5, NN/32);
    }   
#endif
    
    //std::cout << "shutdown"<< std::endl;
    //pool.shutdown();
    std::cout << "end"<< std::endl;
    return 0;
}
#endif
