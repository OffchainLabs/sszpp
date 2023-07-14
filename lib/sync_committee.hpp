/*  sync_committee.hpp
 *
 *  This file is part of ssz++.
 *  ssz++ is a C++ library implementing simple serialize
 *  https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 *  Copyright (c) 2023 - Offchain Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *  http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
