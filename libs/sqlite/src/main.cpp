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
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/fusion/include/io.hpp>

#include <boost/range/iterator_range.hpp>

#include <boost/tuple/tuple.hpp>

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

    return eggs::sqlite::last_insert_rowid( statement.get_database() );
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

    return eggs::sqlite::changes( statement.get_database() );
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

    return eggs::sqlite::changes( statement.get_database() );
}
template< typename Row >
inline std::size_t delete_( eggs::sqlite::ostatement& statement, Row const& row )
{
    statement << row;
    return delete_( statement );
}

int main( int argc, char* argv[] )
{
    namespace sqlite = eggs::sqlite;
    try
    {
        sqlite::database books_db( "books.db", sqlite::database::mode::read_write | sqlite::database::mode::create );
        sqlite::istatement books_by_author(
            books_db
          , "SELECT title, year FROM books "
            "WHERE author=:author "
            "ORDER BY year ASC"
        );

        // Core access
        books_by_author["author"] = "Bjarne Stroustrup";
        {
            books_by_author.step();
            std::cout
             << "title: " << books_by_author.get< std::string >( 0 ) << ", "
             << "year: " << books_by_author.get< int >( 1 ) << '\n'
             ;
        }
        books_by_author.reset();

        // Row object
        books_by_author["author"] = "Bjarne Stroustrup";
        {
            sqlite::row row_results;

            books_by_author >> row_results;
            std::cout
             << "title: " << row_results.get< std::string >( 0 ) << ", "
             << "year: " << row_results.get< int >( 1 ) << '\n'
             ;
        }
        books_by_author.reset();
        
        // Fusion sequences
        books_by_author["author"] = "Bjarne Stroustrup";
        {
            std::pair< std::string, int > pair_results;

            books_by_author >> pair_results;
            std::cout
             << "title: " << pair_results.first << ", "
             << "year: " << pair_results.second << '\n'
             ;

            std::string title;
            int year;

            books_by_author >> boost::tie( title, year );
            std::cout
             << "title: " << title << ", "
             << "year: " << year << '\n'
             ;
        }
        books_by_author.reset();

        // Result set
        books_by_author["author"] = "Bjarne Stroustrup";
        {
            std::vector< sqlite::row > result_set;
            result_set.assign(
                sqlite::istatement_iterator< sqlite::row >( books_by_author )
              , sqlite::istatement_iterator< sqlite::row >()
            );

            // do something with result_set
        }
        books_by_author.reset();
        

    //} catch( sqlite::sqlite_error const& e ) {
    //    std::cerr
    //        << "something went wrong" "\n"
    //        << boost::diagnostic_information( e ) << "\n"
    //        << e.message() << std::endl
    //        ;
    } catch( std::exception const& e ) {
        std::cerr
            << "something went wrong" "\n"
            << boost::diagnostic_information( e ) << std::endl
            ;
    } catch( ... ) {
        std::cerr
            << "something went really wrong..." << std::endl
            ;
    }

    return 0;
}