/*
Virtual Qmail (http://v-q.foo-baz.com)
Copyright (C) 2002-2005 Pawel Niewiadomski (new@foo-baz.com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __CPGSQLPOOL_HPP
#define __CPGSQLPOOL_HPP

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>
#include <string>

class cpgsqlpool {
		public:
				typedef boost::shared_ptr< pqxx::connection > value_type;
				typedef std::vector< value_type > array_type;
				typedef array_type::size_type size_type;
				typedef char bitmap_value;
				typedef std::vector< bitmap_value > bitmap_type;

				class value_ptr {
						public:
								value_ptr( value_type &v, bitmap_value &iu )
										: val(v), in_use(iu) {
								}
								
								~value_ptr() {
										in_use = 0;
								}

								value_type & get() {
										return val;
								}

						protected:
								value_type &val;
								bitmap_value &in_use;
				};

				inline cpgsqlpool( const std::string &, size_type );
				inline value_ptr get();

		protected:
				boost::mutex mux_get;

				array_type pool;
				bitmap_type in_use;
};

cpgsqlpool::cpgsqlpool( const std::string & info, size_type pool_size ) {
	pool.reserve(pool_size);
	in_use.resize(pool_size); // inialize with default constructor which is 0
	for( size_type i=0; i<pool_size; ++i ) {
			pool.push_back( value_type(new pqxx::connection(info)) );
			if( ! pool.back().get() || ! pool.back()->is_open() ) {
					throw ::vq::except(
						static_cast<const char*>("can't create connection"),
						__FILE__, __LINE__ ); 
			}
	}
}

cpgsqlpool::value_ptr cpgsqlpool::get() {
	boost::mutex::scoped_lock lock_get(mux_get);
	for( ;; ) {
			for( size_type i=0, s=in_use.size(); i<s; ++i ) {
					if( ! in_use[i] ) {
							++in_use[i];
							return value_ptr(pool[i], in_use[i]);
					}
			}
	}
}

#endif // ifndef __CPGSQLPOOL_HPP
