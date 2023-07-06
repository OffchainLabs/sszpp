/*  checkpoint.hpp
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
#include "container.hpp"

namespace ssz {
struct checkpoint_t : ssz_container {
    Epoch epoch;
    Root root;

    constexpr auto operator<=>(const checkpoint_t& rhs) const noexcept = default;
    constexpr bool operator==(const checkpoint_t& rhs) const noexcept = default;

    SSZ_CONT(epoch, root);
    YAML_CONT(std::pair<const char*, Epoch&>("epoch", epoch), std::pair<const char*, Root&>("root", root));
};
}
