/**
 * Eggs.SQLite <eggs/sqlite/blob.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_BLOB_HPP
#define EGGS_SQLITE_BLOB_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>

#include <boost/assert.hpp>

#include <boost/static_assert.hpp>

#include <boost/type_traits/is_pod.hpp>

#include <algorithm>
#include <vector>

namespace eggs { namespace sqlite {

    class blob
    {
    public:
        blob()
          : _value()
        {}

        blob( void const* bytes, std::size_t size )
          : _value(
                static_cast< unsigned char const* >( bytes )
              , static_cast< unsigned char const* >( bytes ) + size
            )
        {}
        template< typename InputIterator >
        blob( InputIterator begin, InputIterator end )
          : _value( begin, end )
        {}

        blob( blob const& right )
          : _value( right._value )
        {}

        void assign( void const* bytes, std::size_t size )
        {
            _value.assign(
                static_cast< unsigned char const* >( bytes )
              , static_cast< unsigned char const* >( bytes ) + size
            );
        }
        template< typename InputIterator >
        void assign( InputIterator begin, InputIterator end )
        {
            _value.assign( begin, end );
        }
        
        void write( std::size_t offset, void const* bytes, std::size_t size )
        {
            _value.insert(
                _value.begin() + offset
              , static_cast< unsigned char const* >( bytes )
              , static_cast< unsigned char const* >( bytes ) + size
            );
        }
        template< typename InputIterator >
        void write( std::size_t offset, InputIterator begin, InputIterator end )
        {
            _value.insert(
                _value.begin() + offset
              , begin, end
            );
        }
        
        void read( std::size_t offset, void* bytes, std::size_t size )
        {
            std::copy(
                _value.begin() + offset, _value.begin() + offset + size
              , static_cast< unsigned char* >( bytes )
            );
        }
        template< typename OutputIterator >
        void read( std::size_t offset, OutputIterator begin, std::size_t size )
        {
            std::copy(
                _value.begin() + offset, _value.begin() + offset + size
              , begin
            );
        }

        void clear()
        {
            _value.clear();
        }

        std::size_t size() const
        {
            return _value.size();
        }

        unsigned char const* bytes() const
        {
            return _value.data();
        }

    private:
        std::vector< unsigned char > _value;
    };

    template< typename Type >
    void write_blob( blob& blob, Type const& value )
    {
        BOOST_STATIC_ASSERT(( boost::is_pod< Type >::value ));

        blob.assign(
            reinterpret_cast< unsigned char* >( &value )
          , sizeof( value )
        );
    }

    template< typename Type >
    void read_blob( blob const& blob, Type& value )
    {
        BOOST_STATIC_ASSERT(( boost::is_pod< Type >::value ));

        std::copy(
            blob.bytes(), blob.bytes() + blob.size()
          , reinterpret_cast< unsigned char* >( &value )
        );
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_BLOB_HPP*/
