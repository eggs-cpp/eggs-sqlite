/**
 * Eggs.SQLite <eggs/sqlite/pragma.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_PRAGMA_HPP
#define EGGS_SQLITE_PRAGMA_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/database.hpp>
#include <eggs/sqlite/statement.hpp>

#include <boost/optional.hpp>

#include <boost/type_traits/is_void.hpp>

#include <boost/utility/enable_if.hpp>

#include <iosfwd>
#include <sstream>
#include <string>
#include <vector>

namespace eggs { namespace sqlite {

    namespace detail {

        struct pragma_value_read_only
        {
            template< typename Derived >
            struct param
            {
                typedef void type;
            };

            template< typename Derived >
            struct result
            {
                typedef typename Derived::value_type type;
            };
        };

        struct pragma_value_write_only
        {
            template< typename Derived >
            struct param
            {
                typedef typename Derived::value_type type;
            };

            template< typename Derived >
            struct result
            {
                typedef void type;
            };
        };

        struct pragma_value_read_write
        {
            template< typename Derived >
            struct param
            {
                typedef typename Derived::value_type type;
            };

            template< typename Derived >
            struct result
            {
                typedef typename Derived::value_type type;
            };
        };

        struct pragma_value_list
        {
            template< typename Derived >
            struct param
            {
                typedef void type;
            };

            template< typename Derived >
            struct result
            {
                typedef std::vector< typename Derived::value_type > type;
            };
        };

        struct pragma_procedure
        {
            template< typename Derived >
            struct param
            {
                typedef typename Derived::param_type type;
            };

            template< typename Derived >
            struct result
            {
                typedef void type;
            };
        };

        struct pragma_function_list
        {
            template< typename Derived >
            struct param
            {
                typedef typename Derived::param_type type;
            };

            template< typename Derived >
            struct result
            {
                typedef std::vector< typename Derived::result_type > type;
            };
        };

        void invoke_pragma( database& db, std::string const& query )
        {
            istatement pragma_statement( db, query );

            pragma_statement.step();
        }
        template< typename Type >
        void invoke_pragma( database& db, std::string const& query, Type& value )
        {
            istatement pragma_statement( db, query );

            pragma_statement >> value;
        }
        template< typename Type >
        void invoke_pragma( database& db, std::string const& query, std::vector< Type >& value )
        {
            istatement pragma_statement( db, query );

            while( !pragma_statement.status() == istatement::status_code::done )
            {
                Type element;
                pragma_statement >> element;

                value.push_back( element );
            }
        }

    } // namespace detail

    namespace pragma {

        struct auto_vacuum
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                none = 0
              , full = 1
              , incremental = 2
            };

            static char const* name(){ return "auto_vacuum"; }
        };

        struct automatic_index
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "automatic_index"; }
        };

        struct cache_size
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "cache_size"; }
        };

        struct case_sensitive_like
          : detail::pragma_value_write_only
        {
            typedef bool value_type;

            static char const* name(){ return "case_sensitive_like"; }
        };

        struct checkpoint_fullfsync
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "checkpoint_fullfsync"; }
        };

        struct collation_list
          : detail::pragma_value_list
        {
            typedef std::string value_type;

            static char const* name(){ return "collation_list"; }
        };

        struct compile_options
          : detail::pragma_value_list
        {
            typedef std::string value_type;

            static char const* name(){ return "compile_options"; }
        };

        struct database_list
          : detail::pragma_value_list
        {
            typedef std::string value_type;

            static char const* name(){ return "database_list"; }
        };

        struct encoding
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                utf8
              , utf16
              , utf16le
              , utf16be
            };

            friend void extract( istatement& left, value_type right )
            {
                std::string const& value = left.get< std::string >( 0 );
                if( value == "UTF-8" )
                    right = utf8;
                else if( value == "UTF-16" )
                    right = utf16;
                else if( value == "UTF-16le" )
                    right = utf16le;
                else if( value == "UTF-16be" )
                    right = utf16be;
            }

            template< typename Elem, typename Traits >
            friend std::basic_ostream< Elem, Traits >&
            operator <<( std::basic_ostream< Elem, Traits >& left, value_type const right )
            {
                switch( right )
                {
                case utf8:
                    left << "UTF-8"; break;
                case utf16:
                    left << "UTF-16"; break;
                case utf16le:
                    left << "UTF-16le"; break;
                case utf15be:
                    left << "UTF-16be"; break;
                }

                return left;
            }

            static char const* name(){ return "encoding"; }
        };

        struct foreign_key_list
          : detail::pragma_function_list
        {
            typedef std::string param_type;
            typedef std::string result_type;

            static char const* name(){ return "foreign_key_list"; }
        };

        struct foreign_keys
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "foreign_keys"; }
        };

        struct freelist_count
          : detail::pragma_value_read_only
        {
            typedef int value_type;

            static char const* name(){ return "freelist_count"; }
        };

        struct fullfsync
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "fullfsync"; }
        };

        struct ignore_check_constraints
          : detail::pragma_value_write_only
        {
            typedef bool value_type;

            static char const* name(){ return "ignore_check_constraints"; }
        };

        struct incremental_vacuum
          : detail::pragma_procedure
        {
            typedef int param_type;

            static char const* name(){ return "incremental_vacuum"; }
        };

        struct index_info
          : detail::pragma_function_list
        {
            typedef std::string param_type;
            struct result_type
            {
                int index_rank;
                int table_rank;
                std::string column;

                friend void extract( istatement& left, result_type& right )
                {
                    right.index_rank = left.get< int >( 0 );
                    right.table_rank = left.get< int >( 1 );
                    right.column = left.get< std::string >( 2 );
                }
            };
        
            static char const* name(){ return "index_info"; }
        };

        struct index_list
          : detail::pragma_function_list
        {
            typedef std::string param_type;
            struct result_type
            {
                int rank;
                std::string name;
                bool unique;

                friend void extract( istatement& left, result_type& right )
                {
                    right.rank = left.get< int >( 0 );
                    right.name = left.get< std::string >( 1 );
                    right.unique = left.get< bool >( 2 );
                }
            };
        
            static char const* name(){ return "index_list"; }
        };

        struct integrity_check
          : detail::pragma_function_list
        {
            typedef int param_type;
            typedef std::string result_type;

            static char const* name(){ return "integrity_check"; }
        };

        struct journal_mode
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                delete_
              , truncate
              , persist
              , memory
              , wal
              , off
            };

            friend void extract( istatement& left, value_type right )
            {
                std::string const& value = left.get< std::string >( 0 );
                if( value == "DELETE" )
                    right = delete_;
                else if( value == "TRUNCATE" )
                    right = truncate;
                else if( value == "PERSIST" )
                    right = persist;
                else if( value == "MEMORY" )
                    right = memory;
                else if( value == "WAL" )
                    right = wal;
                else if( value == "OFF" )
                    right = off;
            }

            template< typename Elem, typename Traits >
            friend std::basic_ostream< Elem, Traits >&
            operator <<( std::basic_ostream< Elem, Traits >& left, value_type const right )
            {
                switch( right )
                {
                case delete_:
                    left << "DELETE"; break;
                case truncate:
                    left << "TRUNCATE"; break;
                case persist:
                    left << "PERSIST"; break;
                case memory:
                    left << "MEMORY"; break;
                case wal:
                    left << "WAL"; break;
                case off:
                    left << "OFF"; break;
                }

                return left;
            }

            static char const* name(){ return "journal_mode"; }
        };

        struct journal_size_limit
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "journal_size_limit"; }
        };

        struct legacy_file_format
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "legacy_file_format"; }
        };

        struct locking_mode
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                normal
              , exclusive
            };

            friend void extract( istatement& left, value_type& right )
            {
                std::string const& value = left.get< std::string >( 0 );
                if( value == "NORMAL" )
                    right = normal;
                else if( value == "EXCLUSIVE" )
                    right = exclusive;
            }

            template< typename Elem, typename Traits >
            friend std::basic_ostream< Elem, Traits >&
            operator <<( std::basic_ostream< Elem, Traits >& left, value_type const right )
            {
                switch( right )
                {
                case normal:
                    left << "NORMAL"; break;
                case exclusive:
                    left << "EXCLUSIVE"; break;
                }

                return left;
            }

            static char const* name(){ return "locking_mode"; }
        };

        struct max_page_count
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "max_page_count"; }
        };

        struct page_count
          : detail::pragma_value_read_only
        {
            typedef int value_type;

            static char const* name(){ return "page_count"; }
        };

        struct page_size
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "page_size"; }
        };

#       ifdef SQLITE_DEBUG
        struct parser_trace
          : detail::pragma_value_write_only
        {
            typedef bool value_type;

            static char const* name(){ return "parser_trace"; }
        };
#       endif /*SQLITE_DEBUG*/

        struct quick_check
          : detail::pragma_function_list
        {
            typedef int param_type;
            typedef std::string result_type;

            static char const* name(){ return "quick_check"; }
        };

        struct read_uncommited
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "read_uncommited"; }
        };

        struct recursive_triggers
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "recursive_triggers"; }
        };

        struct reverse_unordered_selects
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "reverse_unordered_selects"; }
        };

        struct schema_version
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "schema_version"; }
        };

        struct user_version
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "user_version"; }
        };

        struct secure_delete
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "secure_delete"; }
        };

        struct shrink_memory
          : detail::pragma_procedure
        {
            typedef void param_type;

            static char const* name(){ return "short_column_names"; }
        };

        struct synchronous
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                off = 0
              , normal = 1
              , full = 2
            };

            static char const* name(){ return "synchronous"; }
        };

        struct table_info
          : detail::pragma_function_list
        {
            typedef std::string param_type;
            struct result_type
            {
                int index;
                std::string name;
                std::string datatype;
                bool not_null;
                boost::optional< std::string > default_value;
                bool primary_key;

                friend void extract( istatement& left, result_type& right )
                {
                    right.index = left.get< int >( 0 );
                    right.name = left.get< std::string >( 1 );
                    right.datatype = left.get< std::string >( 2 );
                    right.not_null = left.get< bool >( 3 );
                    right.default_value = left.get< boost::optional< std::string > >( 4 );
                    right.primary_key = left.get< bool >( 5 );
                }
            };

            static char const* name(){ return "table_info"; }
        };

        struct temp_store
          : detail::pragma_value_read_write
        {
            enum value_type
            {
                default_ = 0
              , file = 1
              , memory = 2
            };

            static char const* name(){ return "temp_store"; }
        };

#       ifdef SQLITE_DEBUG
        struct vbde_listing
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "vbde_listing"; }
        };
#       endif /*SQLITE_DEBUG*/

#       ifdef SQLITE_DEBUG
        struct vbde_trace
          : detail::pragma_value_read_write
        {
            typedef bool value_type;

            static char const* name(){ return "vbde_trace"; }
        };
#       endif /*SQLITE_DEBUG*/

        struct wal_autocheckpoint
          : detail::pragma_value_read_write
        {
            typedef int value_type;

            static char const* name(){ return "wal_autocheckpoint"; }
        };

        struct wal_checkpoint
          : detail::pragma_procedure
        {
            enum param_type
            {
                passive
              , full
              , restart
            };

            template< typename Elem, typename Traits >
            friend std::basic_ostream< Elem, Traits >&
            operator <<( std::basic_ostream< Elem, Traits >& left, param_type const right )
            {
                switch( right )
                {
                case passive:
                    left << "PASSIVE"; break;
                case full:
                    left << "FULL"; break;
                case restart:
                    left << "RESTART"; break;
                }

                return left;
            }

            static char const* name(){ return "wal_checkpoint"; }
        };

        struct writable_schema
          : detail::pragma_value_write_only
        {
            typedef bool value_type;

            static char const* name(){ return "writable_schema"; }
        };

    } // namespace pragma

    template< typename Pragma >
    inline typename Pragma::template result< Pragma >::type
    get_pragma( database& db )
    {
        std::ostringstream query;
        query << "PRAGMA " << Pragma::name() << ";";

        typename Pragma::template result< Pragma >::type result;
        detail::invoke_pragma( db, query.str(), result );

        return result;
    }
    template< typename Pragma >
    inline void set_pragma( database& db, typename Pragma::value_type value )
    {
        std::ostringstream query;
        query << "PRAGMA " << Pragma::name() << "=" << value << ";";

        detail::invoke_pragma( db, query.str() );
    }

    template< typename Pragma >
    inline typename boost::enable_if<
        boost::is_void<
            typename Pragma::template param< Pragma >::type
        >
      , typename Pragma::template result< Pragma >::type
    >
    call_pragma( database& db )
    {
        std::ostringstream query;
        query << "PRAGMA " << Pragma::name() << ";";

        detail::invoke_pragma( db, query.str() );
    }
    template< typename Pragma >
    inline typename boost::disable_if<
        boost::is_void<
            typename Pragma::template param< Pragma >::type
        >
      , typename Pragma::template result< Pragma >::type
    >
    call_pragma( database& db, typename Pragma::template param< Pragma >::type const& param )
    {
        std::ostringstream query;
        query << "PRAGMA " << Pragma::name() << "(" << param << ");";

        typename Pragma::template result< Pragma >::type result;
        detail::invoke_pragma( db, query.str(), result );

        return result;
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_ERROR_HPP*/
