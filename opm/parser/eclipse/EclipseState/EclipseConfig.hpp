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

#ifndef OPM_ECLIPSE_CONFIG_HPP
#define OPM_ECLIPSE_CONFIG_HPP

#include <memory>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/InitConfig/InitConfig.hpp>
#include <opm/parser/eclipse/EclipseState/IOConfig/IOConfig.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>

namespace Opm {
    class EclipseConfig
    {
    public:
        EclipseConfig(const Deck& deck,
                      const ParseContext& parseContext,
                      const Eclipse3DProperties& eclipse3DProperties,
                      std::shared_ptr< EclipseGrid > inputGrid,
                      const Schedule& schedule);

        std::shared_ptr< const IOConfig > getIOConfigConst() const;
        std::shared_ptr< IOConfig > getIOConfig() const;
        std::shared_ptr< const InitConfig > getInitConfig() const;
        std::shared_ptr< const SimulationConfig > getSimulationConfig() const;
        const SummaryConfig& getSummaryConfig() const;


        void initIOConfigPostSchedule(const Deck& deck, const Schedule& schedule);
    private:
        std::shared_ptr<IOConfig> m_ioConfig;
        std::shared_ptr<const InitConfig> m_initConfig;
        std::shared_ptr<const SimulationConfig> m_simulationConfig;
        SummaryConfig m_summaryConfig;
    };
}

#endif // OPM_ECLIPSE_CONFIG_HPP
