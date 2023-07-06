/*  beaconchain.hpp
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

#include <cstdint>
namespace ssz {
using Slot = std::uint64_t;  // If you want a typesafe slot, use enum
using Epoch = std::uint64_t;
using ValidatorIndex = std::uint64_t;
using Root = std::array<std::byte, 32>;
using signature_t = std::array<std::byte, 96>;
using bls_pubkey_t = std::array<std::byte, 48>;
using execution_address_t = std::array<std::byte, 20>;
using Gwei = std::uint64_t;
}  // namespace ssz
