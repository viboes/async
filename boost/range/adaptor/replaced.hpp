// Boost.Range library
//
//  Copyright Neil Groves 2007. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_REPLACED_IMPL_HPP_INCLUDED
#define BOOST_RANGE_ADAPTOR_REPLACED_IMPL_HPP_INCLUDED

#include <boost/config.hpp>
#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/value_type.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/transform_iterator.hpp>

namespace boost
{
	namespace range_detail
	{
		template< class Value >
		class replace_value
		{
		public:
			typedef const Value& result_type;
			typedef const Value& first_argument_type;

			replace_value(const Value& from, const Value& to)
				:	_from(from), _to(to)
			{
			}

			const Value& operator()(const Value& x) const
			{
				return (x == _from) ? _to : x;
			}

		private:
			Value _from;
			Value _to;
		};

		template< class R >
		class replace_range :
			public boost::iterator_range<
				boost::transform_iterator<
					replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type >,
					BOOST_DEDUCED_TYPENAME range_iterator<R>::type > >
		{
		private:
			typedef replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type > Fn;

			typedef boost::iterator_range<
				boost::transform_iterator<
					replace_value< BOOST_DEDUCED_TYPENAME range_value<R>::type >,
					BOOST_DEDUCED_TYPENAME range_iterator<R>::type > > base_t;

		public:
			typedef BOOST_DEDUCED_TYPENAME range_value<R>::type value_type;

			replace_range( R& r, value_type from, value_type to )
				: base_t( make_transform_iterator( boost::begin(r), Fn(from, to) ),
						  make_transform_iterator( boost::end(r), Fn(from, to) ) )
			{ }
		};

		template< class T >
		class replace_holder : public holder2<T>
		{
		public:
			replace_holder( const T& from, const T& to )
				: holder2<T>(from, to)
			{ }
		private:
			// not assignable
			void operator=(const replace_holder&);
		};

		template< class InputRng >
		inline replace_range<InputRng>
		operator|( InputRng& r,
				   const replace_holder<BOOST_DEDUCED_TYPENAME range_value<InputRng>::type>& f )
		{
			return replace_range<InputRng>(r, f.val1, f.val2);
		}

		template< class InputRng >
		inline replace_range<const InputRng>
		operator|( const InputRng& r,
				   const replace_holder<BOOST_DEDUCED_TYPENAME range_value<InputRng>::type>& f )
		{
			return replace_range<const InputRng>(r, f.val1, f.val2);
		}
	} // 'range_detail'

    using range_detail::replace_range;

	namespace adaptors
	{
		namespace
		{
			const range_detail::forwarder2<range_detail::replace_holder>
				replaced =
					range_detail::forwarder2<range_detail::replace_holder>();
		}
	} // 'adaptors'
	
	template<class InputRng>
	inline replace_range<InputRng>
	make_replaced_range(InputRng& rng,
	                    BOOST_DEDUCED_TYPENAME range_value<InputRng>::type from,
	                    BOOST_DEDUCED_TYPENAME range_value<InputRng>::type to)
    {
        return rng | adaptors::replaced(from, to);
    }
    
    template<class InputRng>
    inline replace_range<const InputRng>
    make_replaced_range(const InputRng& rng,
                        BOOST_DEDUCED_TYPENAME range_value<InputRng>::type from,
                        BOOST_DEDUCED_TYPENAME range_value<InputRng>::type to)
    {
        return rng | adaptors::replaced(from, to);
    }
} // 'boost'

#endif // include guard
