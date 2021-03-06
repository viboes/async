//  Copyright (c) 2008 Oliver Kowalke. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TP_EXCEPTIONS_H
#define BOOST_TP_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace boost { namespace tp
{
class invalid_poolsize
: public std::invalid_argument
{
public:
    invalid_poolsize( std::string const& msg)
	: std::invalid_argument( msg)
	{}
};

class invalid_scanns
: public std::invalid_argument
{
public:
    invalid_scanns( std::string const& msg)
	: std::invalid_argument( msg)
	{}
};

class invalid_timeduration
: public std::invalid_argument
{
public:
    invalid_timeduration( std::string const& msg)
	: std::invalid_argument( msg)
	{}
};

class invalid_watermark
: public std::invalid_argument
{
public:
    invalid_watermark( std::string const& msg)
	: std::invalid_argument( msg)
	{}
};

class task_rejected
: public std::runtime_error
{
public:
    task_rejected( std::string const& msg)
	: std::runtime_error( msg)
	{}
};
} }

#endif // BOOST_TP_EXCEPTIONS_H
