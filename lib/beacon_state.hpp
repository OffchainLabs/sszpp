/*  beacon_state.hpp
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

#include "fork.hpp"
#include "validator.hpp"
#include "historical_summary.hpp"
#include "beacon_block.hpp"
#include "sync_committee.hpp"

namespace ssz {

constexpr std::size_t SLOTS_PER_HISTORICAL_ROOT{8192};
constexpr std::size_t HISTORICAL_ROOTS_LIMIT{1 << 24};
constexpr std::size_t SLOTS_PER_EPOCH{32};
constexpr std::size_t EPOCHS_PER_ETH1_VOTING_PERIOD{64};
constexpr std::size_t EPOCHS_PER_SLASHINGS_VECTOR{8192};
constexpr std::size_t EPOCHS_PER_HISTORICAL_VECTOR{1ull << 16};
constexpr std::size_t JUSTIFICATION_BITS_LENGTH{4};
constexpr std::size_t VALIDATOR_REGISTRY_LIMIT{1ull << 40};

using participation_flags_t = std::uint8_t;

struct beacon_state_t : ssz_variable_size_container {
    // Versioning
    std::uint64_t genesis_time;
    Root genesis_validators_root;
    Slot slot;
    fork_t fork;

    // History
    beacon_block_header_t latest_block_header;
    std::array<Root, SLOTS_PER_HISTORICAL_ROOT> block_roots, state_roots;
    ssz::list<Root, HISTORICAL_ROOTS_LIMIT> historical_roots;

    // Eth1
    eth1_data_t eth1_data;
    ssz::list<eth1_data_t, EPOCHS_PER_ETH1_VOTING_PERIOD * SLOTS_PER_EPOCH> eth1_data_votes;
    std::uint64_t eth1_deposit_index;

    // Registry
    ssz::list<validator_t, VALIDATOR_REGISTRY_LIMIT> validators;
    ssz::list<Gwei, VALIDATOR_REGISTRY_LIMIT> balances;

    // Randomness
    std::array<Root, EPOCHS_PER_HISTORICAL_VECTOR> randao_mixes;

    // Slashings
    std::array<Gwei, EPOCHS_PER_SLASHINGS_VECTOR> slashings;

    // Participation
    ssz::list<participation_flags_t, VALIDATOR_REGISTRY_LIMIT> previous_epoch_participation,
        current_epoch_participation;

    // Finality
    std::bitset<JUSTIFICATION_BITS_LENGTH> justification_bits;
    checkpoint_t previous_justified_checkpoint, current_justified_checkpoint, finalized_checkpoint;

    // Inactivity
    ssz::list<std::uint64_t, VALIDATOR_REGISTRY_LIMIT> inactivity_scores;

    // Sync
    sync_committee_t current_sync_committee, next_sync_committee;

    // Execution
    execution_payload_header_t latest_execution_payload_header;

    // Withdrawals
    WithdrawalIndex next_withdrawal_index;
    ValidatorIndex next_withdrawal_validator_index;

    // Deep history valid from Capella onwards
    ssz::list<historical_summary_t, HISTORICAL_ROOTS_LIMIT> historical_summaries;

    constexpr auto operator<=>(const beacon_state_t& rhs) const noexcept = default;
    constexpr bool operator==(const beacon_state_t& rhs) const noexcept = default;

    SSZ_CONT(genesis_time, genesis_validators_root, slot, fork, latest_block_header, block_roots, state_roots,
             historical_roots, eth1_data, eth1_data_votes, eth1_deposit_index, validators, balances, randao_mixes,
             slashings, previous_epoch_participation, current_epoch_participation, justification_bits,
             previous_justified_checkpoint, current_justified_checkpoint, finalized_checkpoint, inactivity_scores,
             current_sync_committee, next_sync_committee, latest_execution_payload_header, next_withdrawal_index,
             next_withdrawal_validator_index, historical_summaries);
    YAML_CONT(std::pair<const char*, std::uint64_t&>("genesis_time", genesis_time),
              std::pair<const char*, Root&>("genesis_validators_root", genesis_validators_root),
              std::pair<const char*, Slot&>("slot", slot), std::pair<const char*, fork_t&>("fork", fork),
              std::pair<const char*, beacon_block_header_t&>("latest_block_header", latest_block_header),
              std::pair<const char*, decltype(block_roots)&>("block_roots", block_roots),
              std::pair<const char*, decltype(state_roots)&>("state_roots", state_roots),
              std::pair<const char*, decltype(historical_roots)&>("historical_roots", historical_roots),
              std::pair<const char*, decltype(eth1_data)&>("eth1_data", eth1_data),
              std::pair<const char*, decltype(eth1_data_votes)&>("eth1_data_votes", eth1_data_votes),
              std::pair<const char*, decltype(eth1_deposit_index)&>("eth1_deposit_index", eth1_deposit_index),
              std::pair<const char*, decltype(validators)&>("validators", validators),
              std::pair<const char*, decltype(balances)&>("balances", balances),
              std::pair<const char*, decltype(randao_mixes)&>("randao_mixes", randao_mixes),
              std::pair<const char*, decltype(slashings)&>("slashings", slashings),
              std::pair<const char*, decltype(previous_epoch_participation)&>("previous_epoch_participation",
                                                                              previous_epoch_participation),
              std::pair<const char*, decltype(current_epoch_participation)&>("current_epoch_participation",
                                                                             current_epoch_participation),
              std::pair<const char*, decltype(justification_bits)&>("justification_bits", justification_bits),
              std::pair<const char*, decltype(previous_justified_checkpoint)&>("previous_justified_checkpoint",
                                                                               previous_justified_checkpoint),
              std::pair<const char*, decltype(current_justified_checkpoint)&>("current_justified_checkpoint",
                                                                              current_justified_checkpoint),
              std::pair<const char*, decltype(finalized_checkpoint)&>("finalized_checkpoint", finalized_checkpoint),
              std::pair<const char*, decltype(inactivity_scores)&>("inactivity_scores", inactivity_scores),
              std::pair<const char*, decltype(current_sync_committee)&>("current_sync_committee",
                                                                        current_sync_committee),
              std::pair<const char*, decltype(next_sync_committee)&>("next_sync_committee", next_sync_committee),
              std::pair<const char*, decltype(latest_execution_payload_header)&>("latest_execution_payload_header",
                                                                                 latest_execution_payload_header),
              std::pair<const char*, decltype(next_withdrawal_index)&>("next_withdrawal_index", next_withdrawal_index),
              std::pair<const char*, decltype(next_withdrawal_validator_index)&>("next_withdrawal_validator_index",
                                                                                 next_withdrawal_validator_index),
              std::pair<const char*, decltype(historical_summaries)&>("historical_summaries", historical_summaries));
};

}  // namespace ssz
