/**
 * Eggs.SQLite <eggs/sqlite/raw_traits.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_RAW_TRAITS_HPP
#define EGGS_SQLITE_RAW_TRAITS_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/blob.hpp>
#include <eggs/sqlite/conversion_traits.hpp>
#include <eggs/sqlite/error.hpp>

#include <boost/cstdint.hpp>

#include <boost/none.hpp>

#include <boost/optional.hpp>

#include <vector>

namespace eggs { namespace sqlite {

    struct storage_class
    {
        enum enum_type
        {
            null = SQLITE_NULL
          , integer = SQLITE_INTEGER
          , real = SQLITE_FLOAT
          , text = SQLITE_TEXT
          , blob = SQLITE_BLOB
        };
    };
    
    template< typename Type >
    struct raw_traits;

    namespace detail {

        template< typename Type, typename RawType >
        struct raw_conversion_traits
        {
            typedef typename conversion_traits< Type >::raw_type value_type;

            static Type get( sqlite3_stmt* statement_handle, std::size_t index )
            {
                return
                    conversion_traits< Type >::from_raw(
                        raw_traits< RawType >::get( statement_handle, index )
                    );
            }
            static void bind( sqlite3_stmt* statement_handle, std::size_t index, Type value )
            {
                raw_traits< RawType >::bind(
                    statement_handle, index
                  , conversion_traits< Type >::to_raw( value )
                );
            }
        };

        template< typename Type >
        struct raw_conversion_traits< Type, void >
        {
            typedef void value_type;
        };

    } // namespace detail
    
    template< typename Type >
    struct raw_traits
      : detail::raw_conversion_traits< Type, typename conversion_traits< Type >::raw_type >
    {};

    template<>
    struct raw_traits< boost::none_t >
    {
        typedef boost::none_t value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            // ( sqlite3_column_type( statement_handle, index ) == SQLITE_NULL ) <-- assert?

            return boost::none;
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_null( statement_handle, index );
        }
    };

    template<>
    struct raw_traits< boost::int32_t >
    {
        typedef boost::int32_t value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            return
                sqlite3_column_int(
                    statement_handle, index
                );
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_int(
                statement_handle, index
              , value
            );
        }
    };

    template<>
    struct raw_traits< boost::int64_t >
    {
        typedef boost::int64_t value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            return
                sqlite3_column_int64(
                    statement_handle, index
                );
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_int64(
                statement_handle, index
              , value
            );
        }
    };
    
    template<>
    struct raw_traits< double >
    {
        typedef double value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            return
                sqlite3_column_double(
                    statement_handle, index
                );
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_double(
                statement_handle, index
              , value
            );
        }
    };

    template<>
    struct raw_traits< char const* >
    {
        typedef char const* value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            return
                static_cast< char const* >(
                    static_cast< void const* >(
                        sqlite3_column_text(
                            statement_handle, index
                        )
                    )
                );
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_text(
                statement_handle, index
              , value, -1, SQLITE_TRANSIENT
            );
        }
    };

    template<>
    struct raw_traits< blob >
    {
        typedef blob value_type;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            void const* bytes = sqlite3_column_blob( statement_handle, index );
            std::size_t const size = sqlite3_column_bytes( statement_handle, index );

            return value_type( bytes, size );
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            sqlite3_bind_blob(
                statement_handle, index
              , value.bytes(), value.size(), SQLITE_TRANSIENT
            );
        }
    };

    template< typename T >
    struct raw_traits< boost::optional< T > >
    {
        typedef boost::optional< T > value_type;
        typedef raw_traits< T > base_traits;

        static value_type get( sqlite3_stmt* statement_handle, std::size_t index )
        {
            if( sqlite3_column_type( statement_handle, index ) != SQLITE_NULL )
            {
                return
                    base_traits::get(
                        statement_handle, index
                    );
            } else {
                return boost::none;
            }
        }
        static void bind( sqlite3_stmt* statement_handle, std::size_t index, value_type value )
        {
            if( value )
            {
                base_traits::bind(
                    statement_handle, index
                  , value
                );
            } else {
                sqlite3_bind_null( statement_handle, index );
            }
        }
    };

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_RAW_TRAITS_HPP*/
