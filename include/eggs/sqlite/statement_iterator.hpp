/**
 * Eggs.SQLite <eggs/sqlite/statement_iterator.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_STATEMENT_ITERATOR_HPP
#define EGGS_SQLITE_STATEMENT_ITERATOR_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/error.hpp>
#include <eggs/sqlite/row.hpp>
#include <eggs/sqlite/statement.hpp>

#include <boost/iterator/iterator_facade.hpp>

#include <cstddef>
#include <iterator>

namespace eggs { namespace sqlite {

    template< typename Row = istatement >
    class istatement_iterator
      : public boost::iterator_facade<
            istatement_iterator< Row >
          , Row const
          , std::input_iterator_tag
        >
    {
    public:
        explicit istatement_iterator()
          : _statement( 0 )
          , _value()
        {}

        explicit istatement_iterator( istatement& statement )
          : _statement( &statement )
          , _value()
        {
            istatement::status_code::enum_type const status =
                _statement->status() == istatement::status_code::reset
                  ? _statement->step()
                  : _statement->status()
                  ;
            if( status == istatement::status_code::row )
            {
                extract( *_statement, _value );
            } else if( status == istatement::status_code::done ) {
                _statement = 0;
            }
        }

        Row const& dereference() const
        {
            return _value;
        }
            
        bool equal( istatement_iterator const& right ) const
        {
            return _statement == right._statement;
        }

        void increment()
        {
            istatement::status_code::enum_type const status =
                _statement->step();
            if( status == istatement::status_code::row )
            {
                extract( *_statement, _value );
            } else if( status == istatement::status_code::done ) {
                _statement = 0;
            }
        }

    private:
        istatement* _statement;
        Row _value;
    };

    template<>
    class istatement_iterator< istatement >
      : public boost::iterator_facade<
            istatement_iterator< istatement >
          , istatement
          , std::input_iterator_tag
        >
    {
    public:
        explicit istatement_iterator()
          : _statement( 0 )
        {}

        explicit istatement_iterator( istatement& statement )
          : _statement( &statement )
        {
            istatement::status_code::enum_type const status =
                _statement->status() == istatement::status_code::reset
                  ? _statement->step()
                  : _statement->status()
                  ;
            if( status == istatement::status_code::done )
            {
                _statement = 0;
            }
        }

        istatement& dereference() const
        {
            return *_statement;
        }
            
        bool equal( istatement_iterator const& right ) const
        {
            return _statement == right._statement;
        }

        void increment()
        {
            istatement::status_code::enum_type const status =
                _statement->step();
            if( status == istatement::status_code::done )
            {
                _statement = 0;
            }
        }

    private:
        istatement* _statement;
    };

    template< typename Row = ostatement >
    class ostatement_iterator
      : public boost::iterator_facade<
            ostatement_iterator< Row >
          , Row
          , std::output_iterator_tag
        >
    {
    public:
        explicit ostatement_iterator()
          : _statement( 0 )
          , _value()
        {}

        explicit ostatement_iterator( ostatement& statement )
          : _statement( &statement )
          , _value()
        {}

        Row& dereference() const
        {
            return _value;
        }
            
        bool equal( ostatement_iterator const& right ) const
        {
            return _statement == right._statement;
        }

        void increment()
        {
            insert( *_statement, _value );
            ostatement::status_code::enum_type const status =
                _statement->step();
        }

    private:
        ostatement* _statement;
        mutable Row _value;
    };

    template<>
    class ostatement_iterator< ostatement >
      : public boost::iterator_facade<
            ostatement_iterator< ostatement >
          , ostatement
          , std::output_iterator_tag
        >
    {
    public:
        explicit ostatement_iterator()
          : _statement( 0 )
        {}

        explicit ostatement_iterator( ostatement& statement )
          : _statement( &statement )
        {}

        ostatement& dereference() const
        {
            return *_statement;
        }
            
        bool equal( ostatement_iterator const& right ) const
        {
            return _statement == right._statement;
        }

        void increment()
        {
            ostatement::status_code::enum_type const status =
                _statement->step();
        }

    private:
        ostatement* _statement;
    };

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_STATEMENT_ITERATOR_HPP*/
