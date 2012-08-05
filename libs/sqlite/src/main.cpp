/**
 * Eggs.SQLite <main.cpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#include <eggs/sqlite/sqlite.hpp>

#include <algorithm>
#include <exception>
#include <iostream>

#include <boost/exception/diagnostic_information.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/fusion/include/tuple.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/range/iterator_range.hpp>

inline boost::iterator_range< eggs::sqlite::istatement_iterator<> >
query( eggs::sqlite::istatement& statement )
{
    return
        boost::make_iterator_range(
            eggs::sqlite::istatement_iterator<>( statement )
          , eggs::sqlite::istatement_iterator<>()
        );
}
template< typename Row >
inline boost::iterator_range< eggs::sqlite::istatement_iterator< Row > >
query( eggs::sqlite::istatement& statement )
{
    return
        boost::make_iterator_range(
            eggs::sqlite::istatement_iterator< Row >( statement )
          , eggs::sqlite::istatement_iterator< Row >()
        );
}

inline boost::int64_t insert( eggs::sqlite::ostatement& statement )
{
    statement.step();

    return statement.get_database().last_insert_rowid();
}
template< typename Row >
inline boost::int64_t insert( eggs::sqlite::ostatement& statement, Row const& row )
{
    statement << row;
    return insert( statement );
}

inline std::size_t update( eggs::sqlite::ostatement& statement )
{
    statement.step();

    return statement.get_database().changes();
}
template< typename Row >
inline std::size_t update( eggs::sqlite::ostatement& statement, Row const& row )
{
    statement << row;
    return update( statement );
}

inline std::size_t delete_( eggs::sqlite::ostatement& statement )
{
    statement.step();

    return statement.get_database().changes();
}
template< typename Row >
inline std::size_t delete_( eggs::sqlite::ostatement& statement, Row const& row )
{
    statement << row;
    return delete( statement );
}

int main( int argc, char* argv[] )
{
    typedef
        boost::fusion::tuple< boost::uint32_t, std::string, std::string >
        note_type;

    namespace sqlite = eggs::sqlite;
    try
    {
        sqlite::database test_db = sqlite::open( "markdown.db" );

        //sqlite::statement test_stmt( test_db, "SELECT * FROM \"sqlite_master\" WHERE type='table'" );
        sqlite::istatement test_stmt( test_db, "SELECT \"_id\", \"Name\", \"Content\" FROM \"Notes\"" );
        sqlite::istatement other_test_stmt( test_db, "SELECT \"_id\", \"Name\", \"Content\" FROM \"Notes\" WHERE \"_id\" = $id" );
        sqlite::ostatement insert_stmt( test_db, "INSERT INTO \"Notes\" (\"Name\", \"Content\") VALUES (?, ?)" );

        //auto params = sqlite3_bind_parameter_count( insert_stmt.native_handle() );
        //auto insert_columns = insert_stmt.columns();

        sqlite::execute( test_db, "DELETE FROM \"Notes\" WHERE \"_id\" >= 20" );

        {
            std::map< std::string, std::string > new_notes;
            new_notes[ "First test" ] = "Some content...";
            new_notes[ "Second test" ] = "Some more content...";
            new_notes[ "Last test" ] = "Last content...";

            std::copy(
                new_notes.cbegin(), new_notes.cend()
              , sqlite::ostatement_iterator< std::pair< std::string, std::string > >( insert_stmt )
            );
        }

        //insert_stmt << std::make_pair( "New", "something..." );
        //insert_stmt.step();

        int version = sqlite::get_pragma< sqlite::pragma::user_version >( test_db );

        auto r = query< note_type >( test_stmt );
        std::vector< note_type > v( r.begin(), r.end() );

        for( 
            sqlite::istatement_iterator< sqlite::istatement >
                iter( test_stmt )
              , end
          ; iter != end
          ; ++iter
        )
        {
            std::cout
                << '#' << iter->get< boost::uint32_t >( 0 ) << ' '
                << iter->get< std::string >( 1 ) << ':' << '\n'
                << iter->get< std::string >( 2 ).substr( 0, 30 ) << "..."
                << '\n' << std::endl;
        }
        
        other_test_stmt[ "id" ] = 19;
        std::copy(
            sqlite::istatement_iterator< note_type >( other_test_stmt ), sqlite::istatement_iterator< note_type >()
          , std::ostream_iterator< note_type >( std::cout, "\n" )
        );

        int breakpoint = 3;
    } catch( sqlite::sqlite_error const& e ) {
        std::cerr
            << "something went wrong :$" "\n"
            << boost::diagnostic_information( e ) << "\n"
            << e.message() << std::endl
            ;
    } catch( std::exception const& e ) {
        std::cerr
            << "something went wrong :$" "\n"
            << boost::diagnostic_information( e ) << std::endl
            ;
    } catch( ... ) {
        std::cerr
            << "something went really wrong..." << std::endl
            ;
    }

    return 0;
}