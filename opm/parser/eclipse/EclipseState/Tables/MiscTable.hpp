/*
  Copyright (C) 2015 Statoil ASA.
  2015 IRIS AS

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
#ifndef OPM_PARSER_MISC_TABLE_HPP
#define	OPM_PARSER_MISC_TABLE_HPP

#include "SimpleTable.hpp"

namespace Opm {
    // forward declaration
    class TableManager;

    class MiscTable : public SimpleTable {
    public:

        friend class TableManager;
        MiscTable() = default;

        /*!
         * \brief Read the MISC keyword and provide some convenience
         *        methods for it.
         */
        void init(Opm::DeckItemConstPtr item)
        {
            SimpleTable::init(item,
                             std::vector<std::string>{
                                 "SolventFraction",
                                 "Miscibility"
                                     });

            SimpleTable::checkNonDefaultable("SolventFraction");
            SimpleTable::checkMonotonic("SolventFraction", /*isAscending=*/true);
            SimpleTable::checkNonDefaultable("Miscibility");
            SimpleTable::checkMonotonic("Miscibility", /*isAscending=*/true, /*isStriclyMonotonic=*/false);
            SimpleTable::checkRange("SolventFraction");
            SimpleTable::checkRange("Miscibility");
        }

#ifdef BOOST_TEST_MODULE
        // DO NOT TRY TO CALL THIS METHOD! it is only for the unit tests!
        void initFORUNITTESTONLY(Opm::DeckItemConstPtr item)
        { init(item); }
#endif

        using SimpleTable::numTables;
        using SimpleTable::numRows;
        using SimpleTable::numColumns;
        using SimpleTable::evaluate;

        const std::vector<double> &getSolventFractionColumn() const
        { return SimpleTable::getColumn(0); }

        const std::vector<double> &getMiscibilityColumn() const
        { return SimpleTable::getColumn(1); }
    };
}

#endif
