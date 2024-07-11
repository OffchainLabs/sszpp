/*  beacon_block.hpp
 *
 *  This file is part of ssz++.
 *  ssz++ is a C++ library implementing simple serialize
 *  https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 *  Copyright (c) 2023 - Potuz potuz@potuz.net
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

#include <cstdint>
#include "beaconchain.hpp"
#include "attestation.hpp"
#include "exits.hpp"
#include "deposit.hpp"
#include "sync_committee.hpp"
#include "withdrawals.hpp"
#include "execution_payload.hpp"

namespace ssz{
constexpr size_t MAX_ATTESTER_SLASHINGS{2};
constexpr size_t MAX_PROPOSER_SLASHINGS{16};
constexpr size_t MAX_BLS_TO_EXECUTION_CHANGES{16};
constexpr size_t MAX_BLOB_COMMITMENTS_PER_BLOCK{4096};
constexpr std::size_t MAX_DEPOSITS{16};

struct eth1_data_t : ssz_container {
    Root deposit_root;
    std::uint64_t deposit_count;
    Root block_hash;

    constexpr auto operator<=>(const eth1_data_t& rhs) const noexcept = default;
    constexpr bool operator==(const eth1_data_t& rhs) const noexcept = default;

    SSZ_CONT(deposit_root, deposit_count, block_hash);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Root&>("deposit_root", deposit_root),
              std::pair<const char*, uint64_t&>("deposit_count", deposit_count),
              std::pair<const char*, Root&>("block_hash", block_hash));
#endif
};

struct beacon_block_header_t : ssz_container {
    Slot slot;
    ValidatorIndex proposer_index;
    Root parent_root, state_root, body_root;

    constexpr auto operator<=>(const beacon_block_header_t& rhs) const noexcept = default;
    constexpr bool operator==(const beacon_block_header_t& rhs) const noexcept = default;

    SSZ_CONT(slot, proposer_index, parent_root, state_root, body_root);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Slot&>("slot", slot),
              std::pair<const char*, ValidatorIndex&>("proposer_index", proposer_index),
              std::pair<const char*, Root&>("parent_root", parent_root),
              std::pair<const char*, Root&>("state_root", state_root),
              std::pair<const char*, Root&>("body_root", body_root));
#endif
};

struct signed_beacon_block_header_t : ssz_container {
    beacon_block_header_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_beacon_block_header_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_beacon_block_header_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, beacon_block_header_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

struct attester_slashing_t : ssz_variable_size_container {
    indexed_attestation_t attestation_1, attestation_2;

    constexpr auto operator<=>(const attester_slashing_t& rhs) const noexcept = default;
    constexpr bool operator==(const attester_slashing_t& rhs) const noexcept = default;

    SSZ_CONT(attestation_1, attestation_2);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, indexed_attestation_t&>("attestation_1", attestation_1),
              std::pair<const char*, indexed_attestation_t&>("attestation_2", attestation_2));
#endif
};

struct proposer_slashing_t : ssz_container {
    signed_beacon_block_header_t signed_header_1, signed_header_2;

    constexpr auto operator<=>(const proposer_slashing_t& rhs) const noexcept = default;
    constexpr bool operator==(const proposer_slashing_t& rhs) const noexcept = default;

    SSZ_CONT(signed_header_1, signed_header_2);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, signed_beacon_block_header_t&>("signed_header_1", signed_header_1),
              std::pair<const char*, signed_beacon_block_header_t&>("signed_header_2", signed_header_2));
#endif
};

struct beacon_block_body_t : ssz_variable_size_container {
    signature_t randao_reveal;
    eth1_data_t eth1_data;
    Root graffiti;
    ssz::list<proposer_slashing_t, MAX_PROPOSER_SLASHINGS> proposer_slashings;
    ssz::list<attester_slashing_t, MAX_ATTESTER_SLASHINGS> attester_slashings;
    ssz::list<attestation_t, MAX_ATTESTATIONS> attestations;
    ssz::list<deposit_t, MAX_DEPOSITS> deposits;
    ssz::list<signed_voluntary_exit_t, MAX_VOLUNTARY_EXITS> voluntary_exits;
    sync_aggregate_t sync_aggregate;
    execution_payload_t execution_payload;
    ssz::list<signed_bls_to_execution_change_t, MAX_BLS_TO_EXECUTION_CHANGES> bls_to_execution_changes;
    ssz::list<blob_kzg_commitments_t, MAX_BLOB_COMMITMENTS_PER_BLOCK> blob_kzg_commitments;

    constexpr auto operator<=>(const beacon_block_body_t& rhs) const noexcept = default;
    constexpr bool operator==(const beacon_block_body_t& rhs) const noexcept = default;

    SSZ_CONT(randao_reveal, eth1_data, graffiti, proposer_slashings, attester_slashings, attestations, deposits,
             voluntary_exits, sync_aggregate, execution_payload, bls_to_execution_changes, blob_kzg_commitments);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, signature_t&>("randao_reveal", randao_reveal),
              std::pair<const char*, eth1_data_t&>("eth1_data", eth1_data),
              std::pair<const char*, Root&>("graffiti", graffiti),
              std::pair<const char*, ssz::list<proposer_slashing_t, MAX_PROPOSER_SLASHINGS>&>("proposer_slashings",
                                                                                              proposer_slashings),
              std::pair<const char*, ssz::list<attester_slashing_t, MAX_ATTESTER_SLASHINGS>&>("attester_slashings",
                                                                                              attester_slashings),
              std::pair<const char*, ssz::list<attestation_t, MAX_ATTESTATIONS>&>("attestations", attestations),
              std::pair<const char*, ssz::list<deposit_t, MAX_DEPOSITS>&>("deposits", deposits),
              std::pair<const char*, ssz::list<signed_voluntary_exit_t, MAX_VOLUNTARY_EXITS>&>("voluntary_exits",
                                                                                               voluntary_exits),
              std::pair<const char*, sync_aggregate_t&>("sync_aggregate", sync_aggregate),
              std::pair<const char*, execution_payload_t&>("execution_payload", execution_payload),
              std::pair<const char*, ssz::list<signed_bls_to_execution_change_t, MAX_BLS_TO_EXECUTION_CHANGES>&>(
                  "bls_to_execution_changes", bls_to_execution_changes),
              std::pair<const char*, ssz::list<blob_kzg_commitments_t, MAX_BLOB_COMMITMENTS_PER_BLOCK>&>(
                  "blob_kzg_commitments", blob_kzg_commitments));
#endif
};

struct beacon_block_t : ssz_variable_size_container {
    Slot slot;
    ValidatorIndex proposer_index;
    Root parent_root, state_root;
    beacon_block_body_t body;

    constexpr auto operator<=>(const beacon_block_t& rhs) const noexcept = default;
    constexpr bool operator==(const beacon_block_t& rhs) const noexcept = default;

    SSZ_CONT(slot, proposer_index, parent_root, state_root, body);

#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Slot&>("slot", slot),
              std::pair<const char*, ValidatorIndex&>("proposer_index", proposer_index),
              std::pair<const char*, Root&>("parent_root", parent_root),
              std::pair<const char*, Root&>("state_root", state_root),
              std::pair<const char*, beacon_block_body_t&>("body", body));
#endif
};

struct signed_beacon_block_t : ssz_variable_size_container {
    beacon_block_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_beacon_block_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_beacon_block_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, beacon_block_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

}  // namespace ssz
