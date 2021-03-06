// Boost.Range library
//
//  Copyright Thorsten Ottosen, Neil Groves 2006 - 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_ADAPTOR_COPIED_HPP
#define BOOST_RANGE_ADAPTOR_COPIED_HPP

#include <boost/range/adaptor/argument_fwd.hpp>
#include <boost/range/adaptor/sliced.hpp>
#include <boost/range/size_type.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost
{
	namespace range_detail
	{
		template< class T >
		struct copy_holder 
			: holder2<std::size_t> 
		{
			copy_holder( std::size_t t, std::size_t u ) 
			    : holder2<std::size_t>(t,u)
			{ }
		};

		template< class CopyableRandomAccessRng, class Int >
		inline CopyableRandomAccessRng 
		operator|( const CopyableRandomAccessRng& r, const copy_holder<Int>& f )
		{
			iterator_range< 
				BOOST_DEDUCED_TYPENAME range_iterator<const 
				                               CopyableRandomAccessRng>::type >
			temp( sliced_impl( r, f ) ); 
			return CopyableRandomAccessRng( temp.begin(), temp.end() );
		}
		
	} // 'range_detail'

	namespace adaptors
	{ 
		namespace
		{
			const range_detail::forwarder2<range_detail::copy_holder> 
  	           copied = range_detail::forwarder2<range_detail::copy_holder>();
		}
	}

	template<class CopyableRandomAccessRng>
	inline CopyableRandomAccessRng
	make_copied_range(const CopyableRandomAccessRng& rng, std::size_t t, std::size_t u)
	{
		return rng | adaptors::copied(t, u);
	}
}

#endif
