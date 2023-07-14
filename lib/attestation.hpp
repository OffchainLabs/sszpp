/*  attestation.hpp
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
#include <cstddef>
#include "checkpoint.hpp"

namespace ssz{
constexpr size_t MAX_ATTESTATIONS{128};

struct attestation_data_t : ssz_container {
    Slot slot;
    ValidatorIndex index;
    Root beacon_block_root;
    checkpoint_t source, target;

    constexpr auto operator<=>(const attestation_data_t& rhs) const noexcept = default;
    constexpr bool operator==(const attestation_data_t& rhs) const noexcept = default;

    SSZ_CONT(slot, index, beacon_block_root, source, target);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Slot&>("slot", slot), std::pair<const char*, ValidatorIndex&>("index", index),
              std::pair<const char*, Root&>("beacon_block_root", beacon_block_root),
              std::pair<const char*, checkpoint_t&>("source", source),
              std::pair<const char*, checkpoint_t&>("target", target));
#endif
};

constexpr std::size_t MAX_VALIDATORS_PER_COMMITEE{2048};
struct attestation_t : ssz_variable_size_container {
    ssz::list<bool, MAX_VALIDATORS_PER_COMMITEE> aggregation_bits;
    attestation_data_t data;
    signature_t signature;

    constexpr auto operator<=>(const attestation_t& rhs) const noexcept = default;
    constexpr bool operator==(const attestation_t& rhs) const noexcept = default;

    SSZ_CONT(aggregation_bits, data, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, ssz::list<bool, MAX_VALIDATORS_PER_COMMITEE>&>("aggregation_bits",
                                                                                    aggregation_bits),
              std::pair<const char*, attestation_data_t&>("data", data),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

struct indexed_attestation_t : ssz_variable_size_container {
    ssz::list<ValidatorIndex, MAX_VALIDATORS_PER_COMMITEE> attesting_indices;
    attestation_data_t data;
    signature_t signature;

    constexpr auto operator<=>(const indexed_attestation_t& rhs) const noexcept = default;
    constexpr bool operator==(const indexed_attestation_t& rhs) const noexcept = default;

    SSZ_CONT(attesting_indices, data, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, ssz::list<ValidatorIndex, MAX_VALIDATORS_PER_COMMITEE>&>("attesting_indices",
                                                                                              attesting_indices),
              std::pair<const char*, attestation_data_t&>("data", data),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

struct aggregate_and_proof_t : ssz_variable_size_container {
    ValidatorIndex aggregator_index;
    attestation_t aggregate;
    signature_t selection_proof;

    constexpr auto operator<=>(const aggregate_and_proof_t& rhs) const noexcept = default;
    constexpr bool operator==(const aggregate_and_proof_t& rhs) const noexcept = default;

    SSZ_CONT(aggregator_index, aggregate, selection_proof);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, ValidatorIndex&>("aggregator_index", aggregator_index),
              std::pair<const char*, attestation_t&>("aggregate", aggregate),
              std::pair<const char*, signature_t&>("selection_proof", selection_proof));
#endif
};

struct signed_aggregate_and_proof_t : ssz_variable_size_container {
    aggregate_and_proof_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_aggregate_and_proof_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_aggregate_and_proof_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, aggregate_and_proof_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};
}  // namespace ssz
