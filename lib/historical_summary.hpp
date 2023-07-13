/*  historical_summary.hpp
 *
 *  This file is part of ssz++.
 *  ssz++ is a C++ library implementing simple serialize
 *  https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 *  Copyright (c) 2023 - Potuz potuz@potuz.net
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "beaconchain.hpp"

namespace ssz {
struct historical_summary_t : ssz_container {
    Root block_summary_root, state_summary_root;

    constexpr auto operator<=>(const historical_summary_t& rhs) const noexcept = default;
    constexpr bool operator==(const historical_summary_t& rhs) const noexcept = default;

    SSZ_CONT(block_summary_root, state_summary_root);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Root&>("block_summary_root", block_summary_root),
              std::pair<const char*, Root&>("state_summary_root", state_summary_root));
#endif
};
}
