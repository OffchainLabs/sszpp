/*  exits.hpp
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
constexpr std::size_t MAX_VOLUNTARY_EXITS{16};

struct voluntary_exit_t : ssz_container {
    Epoch epoch;
    ValidatorIndex validator_index;

    constexpr auto operator<=>(const voluntary_exit_t& rhs) const noexcept = default;
    constexpr bool operator==(const voluntary_exit_t& rhs) const noexcept = default;

    SSZ_CONT(epoch, validator_index);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Epoch&>("epoch", epoch),
              std::pair<const char*, ValidatorIndex&>("validator_index", validator_index));
#endif
};

struct signed_voluntary_exit_t : ssz_container {
    voluntary_exit_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_voluntary_exit_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_voluntary_exit_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, voluntary_exit_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};
}
