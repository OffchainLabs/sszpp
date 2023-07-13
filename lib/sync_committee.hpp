/*  sync_committee.hpp
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
constexpr size_t SYNC_COMMITTEE_SIZE{512};

struct sync_aggregate_t : ssz_container {
    std::bitset<SYNC_COMMITTEE_SIZE> sync_committee_bits;
    signature_t sync_committee_signature;

    constexpr auto operator<=>(const sync_aggregate_t& rhs) const noexcept = default;
    constexpr bool operator==(const sync_aggregate_t& rhs) const noexcept = default;

    SSZ_CONT(sync_committee_bits, sync_committee_signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, std::bitset<SYNC_COMMITTEE_SIZE>&>("sync_committee_bits", sync_committee_bits),
              std::pair<const char*, signature_t&>("sync_committee_signature", sync_committee_signature));
#endif
};

struct sync_committee_t : ssz_container {
    std::array<bls_pubkey_t, SYNC_COMMITTEE_SIZE> pubkeys;
    bls_pubkey_t aggregate_pubkey;

    constexpr auto operator<=>(const sync_committee_t& rhs) const noexcept = default;
    constexpr bool operator==(const sync_committee_t& rhs) const noexcept = default;

    SSZ_CONT(pubkeys, aggregate_pubkey);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, decltype(pubkeys)&>("pubkeys", pubkeys),
              std::pair<const char*, bls_pubkey_t&>("aggregate_pubkey", aggregate_pubkey));
#endif
};
}
