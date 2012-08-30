/**
 * Eggs.SQLite <eggs/sqlite/transaction.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_TRANSACTION_HPP
#define EGGS_SQLITE_TRANSACTION_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/database.hpp>

#include <boost/assert.hpp>

#include <boost/move/move.hpp>

namespace eggs { namespace sqlite {

    class transaction
    {
    public:
        struct mode
        {
            enum enum_type
            {
                deferred
              , immediate
              , exclusive
            };
        };

    public:
        transaction( database& db, mode::enum_type mode = mode::deferred )
          : _db( &db )
          , _mode( mode )
          , _pending( true )
        {
            std::string sql;
            switch( _mode )
            {
            case mode::deferred:
                sql = "BEGIN DEFERRED TRANSACTION;";
                break;
            case mode::immediate:
                sql = "BEGIN IMMEDIATE TRANSACTION;";
                break;
            case mode::exclusive:
                sql = "BEGIN EXCLUSIVE TRANSACTION;";
                break;
            default:
                BOOST_ASSERT(( false ));
                break;
            }

            execute( *_db, sql );
        }
        
    private:
        BOOST_MOVABLE_BUT_NOT_COPYABLE( transaction )

    public:
        transaction( BOOST_RV_REF( transaction ) right )
          : _db( right._db )
          , _mode( right._mode )
          , _pending( right._pending )
        {
            right._db = 0;
            right._pending = false;
        }

        ~transaction()
        {
            if( _pending )
                rollback();
        }
        
        transaction& operator=( BOOST_RV_REF( transaction ) right )
        {
            if( this != &right )
            {
                if( _pending )
                    rollback();

                _db = right._db;
                _mode = right._mode;
                _pending = right._pending;

                right._db = 0;
                right._pending = false;
            }
            return *this;
        }

        void commit()
        {
            BOOST_ASSERT(( _pending ));
            execute( *_db, "COMMIT TRANSACTION" );
        }

        void rollback()
        {
            BOOST_ASSERT(( _pending ));
            execute( *_db, "ROLLBACK TRANSACTION" );
        }

    private:
        database* _db;
        mode::enum_type _mode;
        bool _pending;
    };

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_TRANSACTION_HPP*/
