/**
 * Eggs.SQLite <eggs/sqlite/mutex.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_MUTEX_HPP
#define EGGS_SQLITE_MUTEX_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/database.hpp>
#include <eggs/sqlite/error.hpp>

#include <boost/assert.hpp>

#include <boost/move/move.hpp>

namespace eggs { namespace sqlite {

    class mutex
    {
    public:
        typedef sqlite3_mutex* native_handle_type;

    public:
        mutex()
          : _handle( sqlite3_mutex_alloc( SQLITE_MUTEX_FAST ) )
        {}

        explicit mutex( native_handle_type handle )
          : _handle( handle )
        {
            BOOST_ASSERT(( handle != 0 ));
        }

    private:
        BOOST_MOVABLE_BUT_NOT_COPYABLE( mutex )

    public:
        mutex( BOOST_RV_REF( mutex ) right )
          : _handle( right._handle )
        {
            right._handle = 0;
        }

    public:
        ~mutex()
        {
            sqlite3_mutex_free( _handle );
        }
        
        mutex& operator=( BOOST_RV_REF( mutex ) right )
        {
            if( this != &right )
            {
                sqlite3_mutex_free( _handle );

                _handle = right._handle;

                right._handle = 0;
            }
            return *this;
        }

        void lock()
        {
            sqlite3_mutex_enter( _handle );
        }

        bool try_lock()
        {
            return sqlite3_mutex_try( _handle ) == result_code::ok;
        }

        void unlock()
        {
            sqlite3_mutex_leave( _handle );
        }

        native_handle_type native_handle() const
        {
            return _handle;
        }

    private:
        native_handle_type _handle;
    };

    inline mutex database_mutex( database const& db )
    {
        return
            mutex(
                sqlite3_db_mutex( db.native_handle() )
            );
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_MUTEX_HPP*/
