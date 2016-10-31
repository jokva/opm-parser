/*
  Copyright 2016  Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  OPM is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ostream>
#include <type_traits>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/Runspec.hpp>

namespace Opm {

Phase get_phase( const std::string& str ) {
    if( str == "OIL" ) return Phase::OIL;
    if( str == "GAS" ) return Phase::GAS;
    if( str == "WAT" ) return Phase::WATER;
    if( str == "WATER" ) return Phase::WATER;

    throw std::invalid_argument( "Unknown phase '" + str + "'" );
}

std::ostream& operator<<( std::ostream& stream, const Phase& p ) {
    switch( p ) {
        case Phase::OIL:   return stream << "OIL";
        case Phase::GAS:   return stream << "GAS";
        case Phase::WATER: return stream << "WATER";
    }

    return stream;
}

using un = std::underlying_type< Phase >::type;

Phases::Phases( bool oil, bool gas, bool wat ) noexcept :
    bits( (oil ? (1 << static_cast< un >( Phase::OIL ) )   : 0) |
          (gas ? (1 << static_cast< un >( Phase::GAS ) )   : 0) |
          (wat ? (1 << static_cast< un >( Phase::WATER ) ) : 0) )
{}

bool Phases::active( Phase p ) const noexcept {
    return this->bits[ static_cast< int >( p ) ];
}

size_t Phases::size() const noexcept {
    return this->bits.count();
}

Runspec::Runspec( const Deck& deck ) :
    Runspec( Phases{ deck.hasKeyword( "OIL" ),
                     deck.hasKeyword( "GAS" ),
                     deck.hasKeyword( "WATER" ) } )
{}

Runspec::Runspec( const Phases& p ) noexcept : active_phases( p ) {}

const Phases& Runspec::phases() const noexcept {
    return this->active_phases;
}

}
