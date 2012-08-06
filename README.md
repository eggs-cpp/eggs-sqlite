`Eggs.SQLite`
==============

`Eggs.SQLite` is a thin wrapper over [`SQLite`][sqlite], written following 
_Modern **C++**_ style and making extensive use of the _C++ Standard Library_ 
as well as the [_Boost C++ Libraries_][boost].

The library is in its early development stages, and currently provides no 
examples, tests nor documentation. Its use in a production environment is not 
recommended at the time.

[sqlite]: http://sqlite.org/ "SQLite Home Page"
[boost]: http://boost.org/ "Boost C++ Libraries"

# Basic usage #

## Main components ##

The main components in the library are the `database` that handles the 
connection to a `SQLite` database, and the `statement`s that handles the 
execution of `SQL` statements.

    #include <eggs/sqlite/sqlite.hpp>
    namespace sqlite = eggs::sqlite;
    
    ...

    sqlite::database books_db( "books.db" );
    sqlite::istatement books_by_author(
        books_db
      , "SELECT title, year FROM books "
        "WHERE author=:author "
        "ORDER BY year ASC"
    );

Statements are loosely based on standard streams; an `istatement` reads back 
the results of a `SELECT` statements, and a `ostatement` writes the fields of 
an `INSERT` or `UPDATE` statement.

## Working with data ##

#### core access ####

Input and output statements respectively have a `get` and `put` member 
function providing core access to the statement data. The most basic usage 
includes stepping through a statement (using the member function `step`) and 
retrieving the data using `get`. _Example:_

    books_by_author["author"] = "Bjarne Stroustrup";
    {
        books_by_author.step();
        std::cout
            << "title: " << books_by_author.get< std::string >( 0 ) << ", "
            << "year: " << books_by_author.get< int >( 1 )
            ;
    }
    books_by_author.reset();

#### row objects ####

Statements only hold valid data for the row they are pointing to. Once a 
statement is stepped, previous data becames inaccessible. In particular, for 
columns of type _text_ and _blob_ their buffers became invalidated. The `row` 
object provides a way to extract all data from a statement, data that will not 
be invalidate as the statement is stepped. Otherwise `row`s behave exactly 
like `statement`s. _Example:_

    books_by_author["author"] = "Bjarne Stroustrup";
    {
        sqlite::row row_results;

        books_by_author >> row_results;
        std::cout
            << "title: " << row_results.get< std::string >( 0 ) << ", "
            << "year: " << row_results.get< int >( 1 )
            ;
    }
    books_by_author.reset();


#### _Boost.Fusion_ sequences ####

The indended use of the library is for most data to be accessed using _Fusion_ 
or _Fusion_ adapted sequences. These sequences hold a number of elements of 
arbitrary type, and provide reflection capabilities to each element's index and 
type which allows the library to operate with them requesting no additional 
information. _Example:_

    books_by_author["author"] = "Bjarne Stroustrup";
    {
        std::pair< std::string, int > pair_results;

        books_by_author >> pair_results;
        std::cout
            << "title: " << pair_results.first << ", "
            << "year: " << pair_results.second
            ;

        std::string title;
        int year;

        books_by_author >> boost::tie( title, year );
        std::cout
            << "title: " << title << ", "
            << "year: " << year
            ;
    }
    books_by_author.reset();
    
## Iterators ##

Iterators over `statement`s are provided in the same fashion as standard 
stream objects. These iterators, named `istatement_iterator` and 
`ostatement_iterator`, allow the interoperation with standard containers, 
algorithms, and even third party code. For instance, combining a `std::vector` 
with a `sqlite::row` (or a _Fusion_ sequence) gives what is sometimes referred 
to as a _result set_. _Example:_

    std::vector< sqlite::row > result_set;
    result_set.assign(
        sqlite::istatement_iterator< sqlite::row >( books_by_author )
      , sqlite::istatement_iterator< sqlite::row >()
    );
    books_by_author.reset();

    // do something with result_set

## Customization points ##

The library provides customization points at three different levels:

#### raw_traits ####

These traits handle _raw_ access to the database. They operate directly with 
`SQLite` handles to read or write a single field. Specializations are provided 
for all fundamental `SQLite` datatypes; extending these traits should only be 
needed when working with `SQLite` extensions that provide a new datatype.

#### conversion_traits ####

These traits handle the conversion between a given type and the _raw_ type that 
will be used with `SQLite`. Specializations are provided for all fundamental 
integral types (mapped to `int32_t` or `int64_t`), floating point types (mapped to 
`double`) and string types (mapped to `char const*`). More specializations of 
fundamental and standard types will be added in the future.

#### row extraction/insertion ####

The extraction or insertion of an entire row can be customized by overloading 
functions `void extract( istatement& stmt, T& row )` and 
`void insert( ostatement& stmt, T const& row )`, which are found via _ADL_. 
Both `row` and _Fusion_ sequences access data by providing these overloads.

---

> Copyright _Agustín Bergé_, _Fusion Fenix_ 2012
> 
> Distributed under the Boost Software License, Version 1.0. (See accompanying
> file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
