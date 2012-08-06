/**
 * Eggs.SQLite <eggs/sqlite/database.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_DATABASE_HPP
#define EGGS_SQLITE_DATABASE_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/error.hpp>

#include <boost/assert.hpp>

#include <boost/cstdint.hpp>

#include <boost/move/move.hpp>

#include <boost/throw_exception.hpp>

namespace eggs { namespace sqlite {

    namespace detail {
        
        inline sqlite3* open(
            char const* filename
          , boost::system::error_code* error_code = 0
        )
        {
            sqlite3* handle = 0;
            int const result =
                sqlite3_open_v2(
                    filename, &handle
                  , SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                  , 0
                );
            if( error_code != 0 )
            {
                error_code->assign( result, sqlite_category() );
            } else if( result != result_code::ok ) {
                sqlite3_close( handle );

                BOOST_THROW_EXCEPTION( sqlite_error( result ) );
            }

            return handle;
        }

    } // namespace detail

    class database
    {
    public:
        typedef sqlite3* native_handle_type;

    public:
        explicit database( native_handle_type handle )
          : _handle( handle )
        {
            BOOST_ASSERT(( handle != 0 ));
        }

        explicit database( std::string const& filename )
          : _handle( detail::open( filename.c_str() ) )
        {}
        
    private:
        BOOST_MOVABLE_BUT_NOT_COPYABLE( database )

    public:
        database( BOOST_RV_REF( database ) right )
          : _handle( right._handle )
        {
            right._handle = 0;
        }

        ~database()
        {
            sqlite3_close( _handle );
        }
        
        database& operator=( BOOST_RV_REF( database ) right )
        {
            if( this != &right )
            {
                sqlite3_close( _handle );

                _handle = right._handle;

                right._handle = 0;
            }
            return *this;
        }

        native_handle_type native_handle() const
        {
            return _handle;
        }

    private:
        native_handle_type _handle;
    };

    inline bool operator ==( database const& left, database const& right )
    {
        return left.native_handle() == right.native_handle();
    }
    inline bool operator !=( database const& left, database const& right )
    {
        return !( left == right );
    }

    inline database open( std::string const& filename, boost::system::error_code& error_code )
    {
        sqlite3* handle = detail::open( filename.c_str(), &error_code );

        return database( error_code ? static_cast< sqlite3* >( 0 ) : handle );
    }
    inline database open( std::string const& filename )
    {
        sqlite3* handle = detail::open( filename.c_str() );

        return database( handle );
    }

    inline boost::int64_t last_insert_rowid( database const& db )
    {
        return sqlite3_last_insert_rowid( db.native_handle() );
    }

    inline std::size_t changes( database const& db )
    {
        return sqlite3_changes( db.native_handle() );
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_DATABASE_HPP*/
