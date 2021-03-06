//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_NTH_ELEMENT_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_NTH_ELEMENT_HPP_INCLUDED

#include <boost/concept_check.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/concepts.hpp>
#include <algorithm>

namespace boost
{
    /// \brief template function nth_element
    ///
    /// range-based version of the nth_element std algorithm
    ///
    /// \pre RandomAccessRange is a model of the RandomAccessRangeConcept
    /// \pre BinaryPredicate is a model of the BinaryPredicateConcept
    template<class RandomAccessRange>
    inline void nth_element(RandomAccessRange& rng,
        BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type nth)
    {
        boost::function_requires< RandomAccessRangeConcept<RandomAccessRange> >();
        std::nth_element(boost::begin(rng), nth, boost::end(rng));
    }

    /// \overload
    template<class RandomAccessRange>
    inline void nth_element(const RandomAccessRange& rng,
        BOOST_DEDUCED_TYPENAME range_iterator<const RandomAccessRange>::type nth)
    {
        boost::function_requires< RandomAccessRangeConcept<RandomAccessRange> >();
        std::nth_element(boost::begin(rng),nth,boost::end(rng));
    }

    /// \overload
    template<class RandomAccessRange, class BinaryPredicate>
    inline void nth_element(RandomAccessRange& rng,
        BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type nth,
        BinaryPredicate sort_pred)
    {
        boost::function_requires< RandomAccessRangeConcept<RandomAccessRange> >();
        std::nth_element(boost::begin(rng), nth, boost::end(rng), sort_pred);
    }

    /// \overload
    template<class RandomAccessRange, class BinaryPredicate>
    inline void nth_element(const RandomAccessRange& rng,
        BOOST_DEDUCED_TYPENAME range_iterator<const RandomAccessRange>::type nth,
        BinaryPredicate sort_pred)
    {
        boost::function_requires< RandomAccessRangeConcept<RandomAccessRange> >();
        std::nth_element(boost::begin(rng),nth,boost::end(rng), sort_pred);
    }
}

#endif // include guard
