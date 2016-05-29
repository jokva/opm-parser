/*
  Copyright 2016 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/
#define BOOST_TEST_MODULE FunctionalTests

#include <iostream>
#include <vector>
#include <map>

#include <opm/common/utility/platform_dependent/disable_warnings.h>
#include <boost/test/unit_test.hpp>
#include <opm/common/utility/platform_dependent/reenable_warnings.h>

#include <opm/parser/eclipse/Utility/Functional.hpp>


using namespace Opm;

struct copyable {
    copyable( int v ) : x( v ) {}
    copyable( const copyable& other ) :
        x( other.x ),
        copies( other.copies + 1 ),
        moves( other.moves )
    {}

    copyable( copyable&& other ) :
        x( other.x ),
        copies( other.copies ),
        moves( other.moves + 1 )
    {}

    int x;
    int copies = 0;
    int moves = 0;
};

struct noncopyable {
    noncopyable( int v ) : x( v ) {}
    noncopyable( noncopyable&& other ) :
        x( other.x ),
        moves( other.moves + 1 )
    {}
    noncopyable( const noncopyable& ) = delete;

    int x;
    int moves = 0;
};

copyable plus1move( copyable&& c ) {
    auto next = std::move( c );
    next.x += 1;
    return next;
}

noncopyable plus2move( noncopyable&& c ) {
    auto next = std::move( c );
    next.x += 1;
    return next;
}

copyable plus1copy( const copyable& c ) {
    auto next = c;
    next.x += 1;
    return next;
}

copyable& plus1ref( copyable& c ) {
    c.x += 1;
    return c;
}

copyable mkcopyable( int x ) {
    return { x };
}

noncopyable mkncopyable( int x ) {
    return { x };
}

BOOST_AUTO_TEST_CASE(mapCopy) {
    std::vector< copyable > copyseq = fun::map( mkcopyable, fun::iota( 5 ) );

    auto m = fun::map( plus1copy, copyseq );
    std::vector< copyable > copy { m.begin(), m.end() };

    for( const auto& c : copyseq )
        BOOST_CHECK_EQUAL( c.copies, 0 );

    for( const auto& c : copy )
        BOOST_CHECK_EQUAL( c.copies, 1 );

    for( const auto& c : m )
        BOOST_CHECK_EQUAL( c.copies, 1 );
}

BOOST_AUTO_TEST_CASE(mapMove) {
    std::vector< copyable > moveseq = fun::map( mkcopyable, fun::iota( 5 ) );
    std::vector< noncopyable > ncseq = fun::map( mkncopyable, fun::iota( 5 ) );

    for( const auto& c : moveseq )
        BOOST_CHECK_EQUAL( c.copies, 0 );


    /* class has copy constructor, but move constructor should be used */
    auto m = fun::map( plus1move, std::move( moveseq ) );
    /* should work even in absence of copy constructor */
    auto n = fun::map( plus2move, std::move( ncseq ) );
    std::vector< copyable > cpmove { m.begin(), m.end() };
    std::vector< noncopyable > ncmove { n.begin(), n.end() };

    for( const auto& c : m )
        BOOST_CHECK_EQUAL( c.copies, 0 );
}

BOOST_AUTO_TEST_CASE(mapStdMove) {
    std::vector< copyable > srcseq = fun::map( mkcopyable, fun::iota( 5 ) );
    std::vector< copyable > dstseq;
    // The reserve is important to avoid copies during realloc
    // which would fail the otherwise successful test
    dstseq.reserve( srcseq.size() );

    auto m = fun::map( plus1ref, srcseq );
    std::move( m.begin(), m.end(), std::back_inserter( dstseq ) );

    for( const auto& c : dstseq )
        BOOST_CHECK_EQUAL( c.copies, 0 );
}

BOOST_AUTO_TEST_CASE(TestMap) {
    std::map<std::string, int> m = { {"C", 3}, {"B" , 2} , {"A" , 1}};
    std::vector<std::string> keys_expected = {"A" , "B" , "C"};
    auto keys = fun::map( [] ( const std::pair<std::string,int>& pair) { return pair.first; } , m);

    BOOST_CHECK_EQUAL_COLLECTIONS(keys.begin(), keys.end(),
                                  keys_expected.begin(), keys_expected.end());
}


BOOST_AUTO_TEST_CASE(TestConcat) {
    std::vector<std::vector<int>> vector_of_vectors = {{1},{2,2},{3,3,3}};
    auto conc = fun::concat( std::move(vector_of_vectors) );
    std::vector<int> expected = {1,2,2,3,3,3};

    BOOST_CHECK_EQUAL_COLLECTIONS(conc.begin(), conc.end(),
                                  expected.begin(), expected.end());
}


BOOST_AUTO_TEST_CASE(TestConcatMap) {
    std::vector<int> input = {1,2,3};
    auto conc = fun::concat( fun::map( []( int x ) { return std::vector<int>( x,x ); } , input));

    std::vector<int> expected = {1,2,2,3,3,3};
    BOOST_CHECK_EQUAL_COLLECTIONS(conc.begin(), conc.end(),
                                  expected.begin(), expected.end());

}



BOOST_AUTO_TEST_CASE(iotaEqualCollections) {
    std::vector< int > vec( 5 );

    for( int i = 0; i < 5; ++i )
        vec[ i ] = i;

    fun::iota iota( 5 );
    for( auto x : iota )
        std::cout << x << " ";
    std::cout << std::endl;
    std::vector< int > vec_iota( iota.begin(), iota.end() );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            vec.begin(), vec.end() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            fun::iota( 5 ).begin(), fun::iota( 5 ).end() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec.begin(), vec.end(),
            fun::iota( 5 ).begin(), fun::iota( 5 ).end() );
}

BOOST_AUTO_TEST_CASE(iotaForeach) {
    /* this test is mostly a syntax verification test */

    std::vector< int > vec = { 0, 1, 2, 3, 4 };

    for( auto x : fun::iota( 5 ) )
        BOOST_CHECK_EQUAL( vec[ x ], x );
}

BOOST_AUTO_TEST_CASE(iotaSize) {
    BOOST_CHECK_EQUAL( 5, fun::iota( 5 ).size() );
    BOOST_CHECK_EQUAL( 5, fun::iota( 1, 6 ).size() );
    BOOST_CHECK_EQUAL( 0, fun::iota( 0 ).size() );
    BOOST_CHECK_EQUAL( 0, fun::iota( 0, 0 ).size() );
}

BOOST_AUTO_TEST_CASE(iotaWithMap) {
    const auto plus1 = []( int x ) { return x + 1; };

    std::vector< int > vec = { 1, 2, 3, 4, 5 };
    fun::iota iota( 5 );
    auto vec_iota = fun::map( plus1, iota );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            vec.begin(), vec.end() );
}

BOOST_AUTO_TEST_CASE(iotaNegativeBegin) {
    const auto vec = { -4, -3, -2, -1, 0 };

    fun::iota iota( -4, 1 );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec.begin(), vec.end(),
            iota.begin(), iota.end() );
}
