/**
 * Eggs.SQLite <eggs/sqlite/row.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_ROW_HPP
#define EGGS_SQLITE_ROW_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/error.hpp>
#include <eggs/sqlite/conversion_traits.hpp>
#include <eggs/sqlite/statement.hpp>

#include <boost/none.hpp>

#include <boost/optional.hpp>

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>

#include <cstddef>

#include <string>
#include <vector>

namespace eggs { namespace sqlite {

    namespace detail {

        typedef
            boost::variant<
                boost::none_t
              , int
              , double
              , std::string
              , blob
            >
            row_value_type;

        storage_class::enum_type type( row_value_type const& value )
        {
            static storage_class::enum_type const map[] =
            {
                storage_class::null
              , storage_class::integer
              , storage_class::real
              , storage_class::text
              , storage_class::blob
            };

            return map[ value.which() ];
        }

        inline row_value_type get_value( istatement& statement, std::size_t index )
        {
            switch( statement.type( index ) )
            {
            case storage_class::null:
                return boost::none;
            case storage_class::integer:
                return statement.get< int >( index );
            case storage_class::real:
                return statement.get< double >( index );
            case storage_class::text:
                return statement.get< std::string >( index );
            case storage_class::blob:
                return statement.get< blob >( index );
            }

            // should never reach here
            return boost::none;
        }

        class put_value_visitor
          : public boost::static_visitor< void >
        {
        public:
            explicit put_value_visitor( ostatement& statement, std::size_t index )
              : _statement( &statement )
              , _index( index )
            {}

            template< typename Type >
            void operator()( Type const& value ) const
            {
                _statement->put( _index, value );
            }

        private:
            ostatement* _statement;
            std::size_t _index;
        };
        
        inline void put_value( ostatement& statement, std::size_t index, row_value_type const& value )
        {
            boost::apply_visitor(
                put_value_visitor( statement, index )
              , value
            );
        }

        template< typename RawType >
        struct row_storage
        {
            static RawType get( row_value_type const& storage )
            {
                return
                    boost::get< RawType >( storage );
            }

            template< typename Type >
            static void put( row_value_type& storage, Type const& value )
            {
                storage = value;
            }
        };
        template<>
        struct row_storage< char const* >
        {
            static char const* get( row_value_type const& storage )
            {
                return
                    boost::get< std::string >( storage ).c_str();
            }
            
            template< typename Type >
            static void put( row_value_type& storage, Type const& value )
            {
                storage = std::string( value );
            }
        };
        template< typename RawType >
        struct row_storage< boost::optional< RawType > >
        {
            static boost::optional< RawType > get( row_value_type const& storage )
            {
                if( storage.which() != 0 )
                {
                    return
                        boost::get< RawType >( storage );
                } else {
                    return boost::none;
                }
            }

            template< typename Type >
            static void put( row_value_type& storage, boost::optional< Type > const& value )
            {
                if( value )
                {
                    storage = value;
                } else {
                    storage = boost::none
                }
            }
        };

    } // namespace detail

    class row
    {
    public:
        typedef detail::row_value_type value_type;
        typedef std::vector< value_type >::size_type size_type;

    public:
        explicit row()
          : _values()
        {}
        
        row( row const& right )
          : _values( right._values )
        {}
        
        storage_class::enum_type type( size_type index ) const
        {
            return
                detail::type( _values[ index ] );
        }
        
        template< typename Type >
        Type get( size_type index ) const
        {
            typedef conversion_traits< Type > base_traits;

            return
                base_traits::from_raw(
                    detail::row_storage< typename base_traits::raw_type >::get(
                        _values[ index ]
                    )
                );
        }
        
        template< typename Type >
        void put( size_type index, Type const& value ) const
        {
            typedef conversion_traits< Type > base_traits;
            
            detail::row_storage< typename base_traits::raw_type >::put(
                _values[ index ]
              , value
            );
        }

        size_type size() const
        {
            return _values.size();
        }

        friend inline void extract( istatement& left, row& right )
        {
            std::size_t const column_count = left.columns().size();

            right._values.resize( column_count );
            for( std::size_t i = 0; i != column_count; ++i )
            {
                right._values[i] = detail::get_value( left, i );
            }
        }
        friend inline void insert( ostatement& left, row& right )
        {
            std::size_t const column_count = right._values.size();

            for( std::size_t i = 0; i != column_count; ++i )
            {
                detail::put_value( left, i, right._values[i] );
            }
        }

    private:
        std::vector< value_type > _values;
    };

    typedef std::vector< row > rowset;

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_ROW_HPP*/
