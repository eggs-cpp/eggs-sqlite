/**
 * Eggs.SQLite <eggs/sqlite/statement.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_STATEMENT_HPP
#define EGGS_SQLITE_STATEMENT_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/database.hpp>
#include <eggs/sqlite/error.hpp>
#include <eggs/sqlite/raw_traits.hpp>

#include <boost/assert.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/move/move.hpp>

#include <boost/static_assert.hpp>

#include <boost/throw_exception.hpp>

#include <boost/type_traits/is_void.hpp>

#include <boost/utility/enable_if.hpp>

#include <cstddef>

#include <algorithm>
#include <exception>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace eggs { namespace sqlite {

    namespace detail {

        struct status_code
        {
            enum enum_type
            {
                reset = 0
                , row = result_code::row
                , done = result_code::done
            };
        };

        inline sqlite3_stmt* prepare(
            sqlite3* db_handle
          , char const* sql, int sql_size
          , boost::system::error_code* error_code = 0
        )
        {
            sqlite3_stmt* handle = 0;
            int const result =
                sqlite3_prepare_v2(
                    db_handle
                  , sql, sql_size, &handle
                  , 0 );
            if( error_code != 0 )
            {
                error_code->assign( result, sqlite_category() );
            } else if( result != result_code::ok ) {
                sqlite3_finalize( handle );
                
                BOOST_THROW_EXCEPTION( sqlite_error( result, db_handle ) );
            }

            return handle;
        }

        inline result_code::enum_type step(
            sqlite3_stmt* handle
          , boost::system::error_code* error_code = 0
        )
        {
            int const result =
                sqlite3_step( handle );
            if( error_code != 0 )
            {
                error_code->assign( result, sqlite_category() );
            } else if( result != result_code::ok && !( result == result_code::row || result == result_code::done ) ) {
                BOOST_THROW_EXCEPTION( sqlite_error( result ) );
            }

            return static_cast< result_code::enum_type >( result );
        }

        inline void reset(
            sqlite3_stmt* handle
          , boost::system::error_code* error_code = 0
        )
        {
            int const result =
                sqlite3_reset( handle );
            if( error_code != 0 )
            {
                error_code->assign( result, sqlite_category() );
            } else if( result != result_code::ok ) {
                BOOST_THROW_EXCEPTION( sqlite_error( result ) );
            }
        }

        inline void clear_bindings(
            sqlite3_stmt* handle
          , boost::system::error_code* error_code = 0
        )
        {
            int const result =
                sqlite3_clear_bindings( handle );
            if( error_code != 0 )
            {
                error_code->assign( result, sqlite_category() );
            } else if( result != result_code::ok ) {
                BOOST_THROW_EXCEPTION( sqlite_error( result ) );
            }
        }

        class parameter
        {
        public:
            explicit parameter( sqlite3_stmt* statement_handle, std::size_t index )
              : _statement_handle( statement_handle )
              , _index( index )
            {}

            template< typename Type >
            parameter& operator =( Type value )
            {
                raw_traits< Type >::bind(
                    _statement_handle, _index
                  , value
                );

                return *this;
            }

        private:
            sqlite3_stmt* _statement_handle;
            std::size_t _index;
        };

        inline std::map< std::string, parameter > parameter_map( sqlite3_stmt* statement_handle )
        {
            std::size_t const param_count = sqlite3_bind_parameter_count( statement_handle );

            std::map< std::string, parameter > result;
            for( std::size_t i = 1; i <= param_count; ++i )
            {
                char const* name = sqlite3_bind_parameter_name( statement_handle, i );
                if( name != 0 )
                {
                    result.insert(
                        std::make_pair(
                            name + 1 // strip delimiter
                          , parameter( statement_handle, i )
                        )
                    );
                }
            }

            return result;
        }

        class statement_base
        {
        public:
            typedef sqlite3_stmt* native_handle_type;

            typedef parameter parameter_type;

            typedef status_code status_code;

        public:
            explicit statement_base( database& db, native_handle_type handle )
              : _db( &db )
              , _handle( handle )
              , _status( status_code::reset )
              , _params( parameter_map( _handle ) )
            {}

            explicit statement_base( database& db, std::string const& sql )
              : _db( &db )
              , _handle(
                    detail::prepare(
                        _db->native_handle()
                      , sql.c_str(), sql.size()
                    )
                )
              , _status( status_code::reset )
              , _params( parameter_map( _handle ) )
            {
            }

        private:
            BOOST_COPYABLE_AND_MOVABLE( statement_base )

        public:
            statement_base( statement_base const& right )
              : _db( right._db )
              , _handle(
                    detail::prepare(
                        _db->native_handle()
                      , sqlite3_sql( right.native_handle() ), -1
                    )
                )
              , _status( status_code::reset )
              , _params( right._params )
            {}
            statement_base( BOOST_RV_REF( statement_base ) right )
              : _db( right._db )
              , _handle( right._handle )
              , _status( right._status )
              , _params( boost::move( right._params ) )
            {
                right._db = 0;
                right._handle = 0;
                right._status = status_code::reset;
            }

            ~statement_base()
            {
                sqlite3_finalize( _handle );
            }

            statement_base& operator =( BOOST_COPY_ASSIGN_REF( statement_base ) right )
            {
                _db = right._db;
                _handle =
                    detail::prepare(
                        _db->native_handle()
                      , sqlite3_sql( right.native_handle() ), -1
                    );
                _status = right._status;
                _params = right._params;

                return *this;
            }
            statement_base& operator =( BOOST_RV_REF( statement_base ) right )
            {
                if( this != &right )
                {
                    sqlite3_finalize( _handle );

                    _db = right._db;
                    _handle = right._handle;
                    _status = right._status;
                    _params = boost::move( right._params );
                    
                    right._db = 0;
                    right._handle = 0;
                    right._status = status_code::reset;
                }
                return *this;
            }

            template< typename Type >
            void bind( std::string const& name, Type value )
            {
                BOOST_ASSERT(( _status == status_code::reset ));

                std::map< std::string, parameter_type >::iterator iter
                    = _params.find( name );

                if( iter != _params.end() )
                {
                    iter->second = value;
                } else {
                    throw std::out_of_range( "column name out of range" );
                }
            }

            parameter_type& operator []( std::string const& name )
            {
                BOOST_ASSERT(( _status == status_code::reset ));

                std::map< std::string, parameter_type >::iterator iter
                    = _params.find( name );

                BOOST_ASSERT(( iter != _params.end() ));
                return iter->second;
            }

            void reset()
            {
                detail::reset( _handle );
                detail::clear_bindings( _handle );
                _status = status_code::reset;
            }

            status_code::enum_type status() const
            {
                return _status;
            }

            database& get_database()
            {
                return *_db;
            }

            native_handle_type native_handle() const
            {
                return _handle;
            }

        protected:
            database* _db;
            native_handle_type _handle;        
            status_code::enum_type _status;
            std::map< std::string, parameter_type > _params;
        };

        inline bool operator ==( statement_base const& left, statement_base const& right )
        {
            return left.native_handle() == right.native_handle();
        }
        inline bool operator !=( statement_base const& left, statement_base const& right )
        {
            return !( left == right );
        }

        class column
        {
        public:
            explicit column( std::size_t index, std::string const& name )
              : _index( index )
              , _name( name )
            {}

            std::string name() const
            {
                return _name;
            }

            std::size_t index() const
            {
                return _index;
            }

        private:
            std::size_t _index;
            std::string _name;
        };

        inline std::vector< column > input_columns( sqlite3_stmt* statement_handle )
        {
            std::vector< column > result;

            std::size_t const column_count = sqlite3_column_count( statement_handle );
            result.reserve( column_count );
            for( std::size_t i = 0; i < column_count; ++i )
            {
                char const* name = sqlite3_column_name( statement_handle, i );
                result.push_back( column( i, name ) );
            }

            return result;
        }
        inline std::vector< column > output_columns( sqlite3_stmt* statement_handle )
        {
            std::vector< column > result;
            
            std::size_t const column_count = sqlite3_bind_parameter_count( statement_handle );
            result.reserve( column_count );
            for( std::size_t i = 1; i <= column_count; ++i )
            {
                char const* name = sqlite3_bind_parameter_name( statement_handle, i );
                if( name == 0 )
                {
                    result.push_back( column( i, boost::lexical_cast< std::string >( i ) ) );
                }
            }

            return result;
        }

    } // namespace detail
    
    inline void execute( database& db, std::string const& sql, boost::system::error_code& error_code )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size(), &error_code );
        
        if( !error_code )
            detail::step( handle, &error_code );
    }
    inline void execute( database& db, std::string const& sql )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size() );

        detail::step( handle );
    }

    class istatement
      : public detail::statement_base
    {
    public:
        typedef detail::column column_type;

    public:
        explicit istatement( database& db, native_handle_type handle )
          : detail::statement_base( db, handle )
          , _columns( detail::input_columns( native_handle() ) )
        {}

        explicit istatement( database& db, std::string const& sql )
          : detail::statement_base( db, sql )
          , _columns( detail::input_columns( native_handle() ) )
        {}

    private:
        BOOST_COPYABLE_AND_MOVABLE( istatement )

    public:
        istatement( istatement const& right )
          : detail::statement_base( right )
          , _columns( right._columns )
        {}
        istatement( BOOST_RV_REF( istatement ) right )
          : detail::statement_base( boost::move( right ) )
          , _columns( boost::move( right._columns ) )
        {}

        istatement& operator =( BOOST_COPY_ASSIGN_REF( istatement ) right )
        {
            detail::statement_base::operator =( right );
            _columns = right._columns;

            return *this;
        }
        istatement& operator =( BOOST_RV_REF( istatement ) right )
        {
            if( this != &right )
            {
                detail::statement_base::operator =( boost::move( right ) );
                _columns = boost::move( right._columns );
            }
            return *this;
        }

        std::vector< column_type > const& columns() const
        {
            return _columns;
        }
        
        storage_class::enum_type type( column_type const& column )
        {
            if( _status == status_code::reset )
                step();
            
            BOOST_ASSERT(( _status == status_code::row ));

            return
                static_cast< storage_class::enum_type >(
                    sqlite3_column_type( native_handle(), column.index() )
                );
        }
        storage_class::enum_type type( std::size_t column_index )
        {
            return type( _columns[ column_index ] );
        }
        
        template< typename Type >
        Type get( column_type const& column )
        {
            BOOST_STATIC_ASSERT((
                !boost::is_void<
                    typename raw_traits< Type >::value_type
                 >::value
            ));

            if( _status == status_code::reset )
                step();
            
            BOOST_ASSERT(( _status == status_code::row ));

            return raw_traits< Type >::get( native_handle(), column.index() );
        }
        template< typename Type >
        Type get( std::size_t column_index )
        {
            BOOST_ASSERT(( column_index < _columns.size() ));

            return get< Type >( _columns[ column_index ] );
        }
        template< typename Type >
        Type get( column_type const& column, Type& value )
        {
            return value = get< Type >( column );
        }
        template< typename Type >
        Type get( std::size_t column_index, Type& value )
        {
            return value = get< Type >( _columns[ column_index ] );
        }

        status_code::enum_type step()
        {
            BOOST_ASSERT(( _status != status_code::done ));

            _status = static_cast< status_code::enum_type >( detail::step( _handle ) );
            return _status;
        }

    private:
        std::vector< column_type > _columns;
    };

    template< typename Type >
    inline typename boost::disable_if<
        boost::is_void<
            typename raw_traits< Type >::value_type
        >
    >::type extract( istatement& left, Type& right )
    {
        BOOST_ASSERT(( left.columns().size() == 1 ));

        right = left.get< Type >( 0 );
    }

    template< typename Type >
    istatement& operator >>( istatement& left, Type& right )
    {
        extract( left, right );
        left.step();
    
        return left;
    }

    inline istatement iprepare( database& db, std::string const& sql, boost::system::error_code& error_code )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size(), &error_code );

        return istatement( db, error_code ? static_cast< sqlite3_stmt* >( 0 ) : handle );
    }
    inline istatement iprepare( database& db, std::string const& sql )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size() );

        return istatement( db, handle );
    }

    class ostatement
      : public detail::statement_base
    {
    public:
        typedef detail::column column_type;

    public:
        explicit ostatement( database& db, native_handle_type handle )
          : detail::statement_base( db, handle )
          , _columns( detail::output_columns( native_handle() ) )
        {}

        explicit ostatement( database& db, std::string const& sql )
          : detail::statement_base( db, sql )
          , _columns( detail::output_columns( native_handle() ) )
        {}

    private:
        BOOST_COPYABLE_AND_MOVABLE( ostatement )

    public:
        ostatement( ostatement const& right )
          : detail::statement_base( right )
          , _columns( right._columns )
        {}
        ostatement( BOOST_RV_REF( ostatement ) right )
          : detail::statement_base( boost::move( right ) )
          , _columns( boost::move( right._columns ) )
        {}

        ostatement& operator =( BOOST_COPY_ASSIGN_REF( ostatement ) right )
        {
            detail::statement_base::operator =( right );
            _columns = right._columns;

            return *this;
        }
        ostatement& operator =( BOOST_RV_REF( ostatement ) right )
        {
            if( this != &right )
            {
                detail::statement_base::operator =( boost::move( right ) );
                _columns = boost::move( right._columns );
            }
            return *this;
        }

        std::vector< column_type > const& columns() const
        {
            return _columns;
        }

        template< typename Type >
        void put( column_type const& column, Type const& value ) const
        {
            BOOST_STATIC_ASSERT((
                !boost::is_void<
                    typename raw_traits< Type >::value_type
                 >::value
            ));

            BOOST_ASSERT(( _status == status_code::reset ));

            return raw_traits< Type >::bind( native_handle(), column.index(), value );
        }
        template< typename Type >
        void put( std::size_t column_index, Type const& value ) const
        {
            BOOST_ASSERT(( column_index < _columns.size() ));

            return put< Type >( _columns[ column_index ], value );
        }

        status_code::enum_type step()
        {
            BOOST_ASSERT(( _status == status_code::reset ));
            
            detail::step( _handle );
            detail::reset( _handle );
            _status = status_code::reset;

            return _status;
        }

    private:
        std::vector< column_type > _columns;
    };

    template< typename Type >
    inline typename boost::disable_if<
        boost::is_void<
            typename raw_traits< Type >::value_type
        >
      , ostatement&
    >::type insert( ostatement& left, Type const& right )
    {
        BOOST_ASSERT(( left.columns().size() == 1 ));

        left.put< Type >( 0, right );
    }

    template< typename Type >
    ostatement& operator <<( ostatement& left, Type const& right )
    {
        insert( left, right );
        left.step();
    
        return left;
    }

    inline ostatement oprepare( database& db, std::string const& sql, boost::system::error_code& error_code )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size(), &error_code );

        return ostatement( db, error_code ? static_cast< sqlite3_stmt* >( 0 ) : handle );
    }
    inline ostatement oprepare( database& db, std::string const& sql )
    {
        sqlite3_stmt* handle = detail::prepare( db.native_handle(), sql.c_str(), sql.size() );

        return ostatement( db, handle );
    }

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_STATEMENT_HPP*/
