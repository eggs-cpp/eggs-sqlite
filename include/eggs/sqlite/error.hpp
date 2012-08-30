/**
 * Eggs.SQLite <eggs/sqlite/error.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_ERROR_HPP
#define EGGS_SQLITE_ERROR_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <string>

namespace eggs { namespace sqlite {

    struct result_code
    {
        enum enum_type
        {
            ok = SQLITE_OK /* Successful result */

          , error = SQLITE_ERROR /* SQL error or missing database */
          , internal = SQLITE_INTERNAL /* Internal logic error in SQLite */
          , perm = SQLITE_PERM /* Access permission denied */
          , abort = SQLITE_ABORT /* Callback routine requested an abort */
          , busy = SQLITE_BUSY /* The database file is locked */
          , locked = SQLITE_LOCKED /* A table in the database is locked */
          , no_mem = SQLITE_NOMEM /* A malloc() failed */
          , read_only = SQLITE_READONLY /* Attempt to write a readonly database */
          , interrupt = SQLITE_INTERRUPT /* Operation terminated by sqlite3_interrupt()*/
          , io_error = SQLITE_IOERR /* Some kind of disk I/O error occurred */
          , corrupt = SQLITE_CORRUPT /* The database disk image is malformed */
          , not_found = SQLITE_NOTFOUND /* Unknown opcode in sqlite3_file_control() */
          , full = SQLITE_FULL /* Insertion failed because database is full */
          , cant_open = SQLITE_CANTOPEN /* Unable to open the database file */
          , protocol = SQLITE_PROTOCOL /* Database lock protocol error */
          , empty = SQLITE_EMPTY /* Database is empty */
          , schema = SQLITE_SCHEMA /* The database schema changed */
          , too_big = SQLITE_TOOBIG /* String or BLOB exceeds size limit */
          , contraint = SQLITE_CONSTRAINT /* Abort due to constraint violation */
          , mismatch = SQLITE_MISMATCH /* Data type mismatch */
          , misuse = SQLITE_MISUSE /* Library used incorrectly */
          , no_lfs = SQLITE_NOLFS /* Uses OS features not supported on host */
          , auth = SQLITE_AUTH /* Authorization denied */
          , format = SQLITE_FORMAT /* Auxiliary database format error */
          , range = SQLITE_RANGE /* 2nd parameter to sqlite3_bind out of range */
          , not_a_db = SQLITE_NOTADB /* File opened that is not a database file */
          , row = SQLITE_ROW /* sqlite3_step() has another row ready */
          , done = SQLITE_DONE /* sqlite3_step() has finished executing */
        };
    };

    namespace detail {

        class sqlite_category_impl
          : public boost::system::error_category
        {
        public:
            virtual char const* name() const
            {
                return "sqlite";
            }

            virtual std::string message( int error_code ) const
            {
                switch ( error_code )
                {
                case result_code::ok:
                    return "OK";

                case result_code::error:
                    return "SQL error or missing database";
                case result_code::internal:
                    return "Internal logic error in SQLite";
                case result_code::perm:
                    return "Access permission denied";
                case result_code::abort:
                    return "Callback routine requested an abort";
                case result_code::busy:
                    return "The database file is locked";
                case result_code::locked:
                    return "A table in the database is locked";
                case result_code::no_mem:
                    return "A malloc() failed";
                case result_code::read_only:
                    return "Attempt to write a readonly database";
                case result_code::interrupt:
                    return "Operation terminated by sqlite3_interrupt()";
                case result_code::io_error:
                    return "Some kind of disk I/O error occurred";
                case result_code::corrupt:
                    return "The database disk image is malformed";
                case result_code::not_found:
                    return "Unknown opcode in sqlite3_file_control()";
                case result_code::full:
                    return "Insertion failed because database is full";
                case result_code::cant_open:
                    return "Unable to open the database file";
                case result_code::protocol:
                    return "Database lock protocol error";
                case result_code::empty:
                    return "Database is empty";
                case result_code::schema:
                    return "The database schema changed";
                case result_code::too_big:
                    return "String or BLOB exceeds size limit";
                case result_code::contraint:
                    return "Abort due to constraint violation";
                case result_code::mismatch:
                    return "Data type mismatch";
                case result_code::misuse:
                    return "Library used incorrectly";
                case result_code::no_lfs:
                    return "Uses OS features not supported on host";
                case result_code::auth:
                    return "Authorization denied";
                case result_code::format:
                    return "Auxiliary database format error";
                case result_code::range:
                    return "2nd parameter to sqlite3_bind out of range";
                case result_code::not_a_db:
                    return "File opened that is not a database file";
                case result_code::row:
                    return "sqlite3_step() has another row ready";
                case result_code::done:
                    return "sqlite3_step() has finished executing";

                default:
                    return "Unknown SQLite error";
                }
            }
        };

    } // namespace detail
    
    inline boost::system::error_category const& sqlite_category()
    {
        static detail::sqlite_category_impl category_instance;
        
        return category_instance;
    }
    
    inline boost::system::error_code make_error_code( result_code::enum_type error )
    {
        return
            boost::system::error_code(
                static_cast< int >( error )
              , sqlite_category()
            );
    }

    inline boost::system::error_condition make_error_condition( result_code::enum_type error )
    {
        return
            boost::system::error_condition(
                static_cast< int >( error )
              , sqlite_category()
            );
    }

    class sqlite_error
      : public boost::system::system_error
    {
    public:
        explicit sqlite_error( int result_code )
          : boost::system::system_error(
                boost::system::error_code(
                    result_code
                  , sqlite_category()
                )
            )
        {}
    };

    class sqlite_syntax_error
      : public sqlite_error
    {
    public:
        explicit sqlite_syntax_error( sqlite3* db_handle = 0 )
          : sqlite_error( result_code::error )
          , _message(
                db_handle != 0
                  ? sqlite3_errmsg( db_handle )
                  : 0
            )
        {}

        char const* message() const
        {
            return _message.c_str();
        }

        char const* what() const
        {
            if( _what.empty() )
            {
                try
                {
                    _what = sqlite_error::what();
                    if( !_what.empty() )
                        _what += ": ";
                    _what += _message;
                } catch( ... ) {
                    return sqlite_error::what();
                }
            }
            return _what.c_str();
        }

    private:
        std::string _message;
        mutable std::string _what;
    };

} } // namespace eggs::sqlite

namespace boost { namespace system {

    template<>
    struct is_error_code_enum< eggs::sqlite::result_code::enum_type >
    {
        static const bool value = true;
    };

} } // namespace boost::system

#endif /*EGGS_SQLITE_ERROR_HPP*/
