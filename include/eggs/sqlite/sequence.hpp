/**
 * Eggs.SQLite <eggs/sqlite/sequence.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_SEQUENCE_HPP
#define EGGS_SQLITE_SEQUENCE_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/error.hpp>
#include <eggs/sqlite/statement.hpp>

#include <boost/assert.hpp>

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/iterator.hpp>
#include <boost/fusion/include/next.hpp>
#include <boost/fusion/include/size.hpp>

#include <boost/utility/enable_if.hpp>

#include <cstddef>

namespace eggs { namespace sqlite {

    namespace detail {

        class sequence_extract_fold
        {
        public:
            typedef std::size_t result_type;

        public:
            explicit sequence_extract_fold( istatement& statement )
              : _statement( &statement )
            {}

            template< typename T >
            std::size_t operator ()( std::size_t index, T& v ) const
            {
                v = _statement->get< T >( index );

                return index + 1;
            }

        private:
            istatement* _statement;
        };

        class sequence_insert_fold
        {
        public:
            typedef std::size_t result_type;

        public:
            explicit sequence_insert_fold( ostatement& statement )
              : _statement( &statement )
            {}

            template< typename T >
            std::size_t operator ()( std::size_t index, T const& v ) const
            {
                _statement->put< T >( index, v );

                return index + 1;
            }

        private:
            ostatement* _statement;
        };

    } // namespace detail

    template< typename Sequence >
    inline typename boost::enable_if<
        boost::fusion::traits::is_sequence< Sequence >
    >::type extract( istatement& left, Sequence& right )
    {
        BOOST_ASSERT(( left.columns().size() == boost::fusion::size( right ) ));

        boost::fusion::fold(
            right
          , 0, detail::sequence_extract_fold( left )
        );
    }
    template< typename Sequence >
    inline typename boost::enable_if<
        boost::fusion::traits::is_sequence< Sequence >
    >::type insert( ostatement& left, Sequence const& right )
    {
        BOOST_ASSERT(( left.columns().size() == boost::fusion::size( right ) ));

        boost::fusion::fold(
            right
          , 0, detail::sequence_insert_fold( left )
        );
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_SEQUENCE_HPP*/
