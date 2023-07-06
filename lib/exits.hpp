/*  exits.hpp
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
constexpr std::size_t MAX_VOLUNTARY_EXITS{16};

struct voluntary_exit_t : ssz_container {
    Epoch epoch;
    ValidatorIndex validator_index;

    constexpr auto operator<=>(const voluntary_exit_t& rhs) const noexcept = default;
    constexpr bool operator==(const voluntary_exit_t& rhs) const noexcept = default;

    SSZ_CONT(epoch, validator_index);
    YAML_CONT(std::pair<const char*, Epoch&>("epoch", epoch),
              std::pair<const char*, ValidatorIndex&>("validator_index", validator_index));
};

struct signed_voluntary_exit_t : ssz_container {
    voluntary_exit_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_voluntary_exit_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_voluntary_exit_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
    YAML_CONT(std::pair<const char*, voluntary_exit_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
};
}
