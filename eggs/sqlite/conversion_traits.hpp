/**
 * Eggs.SQLite <eggs/sqlite/conversion_traits.hpp>
 * 
 * Copyright Agustín Bergé, Fusion Fenix 2012
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 * 
 * Library home page: http://github.com/eggs-cpp/eggs-sqlite
 */

#ifndef EGGS_SQLITE_CONVERSION_TRAITS_HPP
#define EGGS_SQLITE_CONVERSION_TRAITS_HPP

#include <eggs/sqlite/detail/sqlite3.hpp>
#include <eggs/sqlite/error.hpp>

#include <boost/cstdint.hpp>

namespace eggs { namespace sqlite {

    template< typename Type, typename Enable = void >
    struct conversion_traits
    {
        typedef Type value_type;
        typedef void raw_type;
    };

    namespace detail {

        template< typename Type, typename Raw >
        struct static_cast_conversion_traits
        {
            typedef Type value_type;
            typedef Raw raw_type;

            static value_type from_raw( raw_type value )
            {
                return static_cast< value_type >( value );
            }
            static raw_type to_raw( value_type value )
            {
                return static_cast< raw_type >( value );
            }
        };

    } // namespace detail

    template<>
    struct conversion_traits< char >
      : detail::static_cast_conversion_traits< char, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::int8_t >
      : detail::static_cast_conversion_traits< boost::int8_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::uint8_t >
      : detail::static_cast_conversion_traits< boost::uint8_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::int16_t >
      : detail::static_cast_conversion_traits< boost::int16_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::uint16_t >
      : detail::static_cast_conversion_traits< boost::uint16_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::int32_t >
      : detail::static_cast_conversion_traits< boost::int32_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::uint32_t >
      : detail::static_cast_conversion_traits< boost::uint32_t, boost::int32_t >
    {};
    template<>
    struct conversion_traits< boost::int64_t >
      : detail::static_cast_conversion_traits< boost::int64_t, boost::int64_t >
    {};
    template<>
    struct conversion_traits< boost::uint64_t >
      : detail::static_cast_conversion_traits< boost::uint64_t, boost::int64_t >
    {};
    
    template<>
    struct conversion_traits< bool >
      : detail::static_cast_conversion_traits< bool, boost::int32_t >
    {
        typedef bool value_type;
        typedef int raw_type;

        static value_type from_raw( raw_type value )
        {
            return value != 0;
        }
        static raw_type to_raw( value_type value )
        {
            return value ? 1 : 0;
        }
    };
    
    template<>
    struct conversion_traits< float >
      : detail::static_cast_conversion_traits< float, double >
    {};
    template<>
    struct conversion_traits< double >
      : detail::static_cast_conversion_traits< double, double >
    {};

    template<>
    struct conversion_traits< std::string >
    {
        typedef std::string value_type;
        typedef char const* raw_type;

        static value_type from_raw( raw_type value )
        {
            return std::string( value != 0 ? value : "" );
        }
        static raw_type to_raw( value_type const& value )
        {
            return value.c_str();
        }
    };

} } // namespace eggs::sqlite

#endif /*EGGS_SQLITE_CONVERSION_TRAITS_HPP*/
