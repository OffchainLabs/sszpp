/*  spec_test.cpp
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


#include <filesystem>
#include <fstream>
#include <memory>

#include "acutest.h"
#include "attestation.hpp"
#include "beacon_block.hpp"
#include "beacon_state.hpp"
#include "container.hpp"
#include "deposit.hpp"
#include "execution_payload.hpp"
#include "exits.hpp"
#include "historical_summary.hpp"
#include "ssz++.hpp"
#include "snappy.h"
#include "sync_committee.hpp"
#include "withdrawals.hpp"
#include "validator.hpp"
#include "fork.hpp"

namespace {
constexpr auto base_path = "tests/mainnet/capella/ssz_static/";
constexpr auto value_file = "/value.yaml";
constexpr auto ssz_file = "/serialized.ssz_snappy";
}

template <typename T>
void do_test(const auto &case_dir) {
    auto node = YAML::LoadFile(case_dir.path().string() + value_file);
    auto decoded = node.template as<T>();
    auto ssz_snappy_path = case_dir.path().string() + ssz_file;
    std::ifstream ssz_snappy(ssz_snappy_path, std::ios::in | std::ios::binary);
    if (not ssz_snappy.is_open())
        throw std::filesystem::filesystem_error("could not open file", case_dir.path(), std::error_code());

    const auto ssz_snappy_size = std::filesystem::file_size(ssz_snappy_path);
    std::vector<std::byte> ssz_content(ssz_snappy_size);
    ssz_snappy.read(reinterpret_cast<char *>(ssz_content.data()), ssz_snappy_size);
    ssz_snappy.close();

    auto serialized_bytes = ssz::serialize(decoded);
    auto deserialized = ssz::deserialize<T>(serialized_bytes);
    TEST_CHECK(deserialized == decoded);
    std::vector<std::byte> ssz_output(serialized_bytes.size());
    if (!snappy::RawUncompress(reinterpret_cast<char *>(ssz_content.data()), ssz_snappy_size,
                               reinterpret_cast<char *>(ssz_output.data())))
        throw std::filesystem::filesystem_error("could not uncompress file", case_dir.path(), std::error_code());

    auto deserialized_snappy = ssz::deserialize<T>(ssz_output);
    TEST_CHECK(deserialized_snappy == decoded);

    auto node_root = YAML::LoadFile(case_dir.path().string() + "/roots.yaml");
    auto root = node_root["root"].template as<ssz::chunk_t>();
    auto htr = ssz::hash_tree_root(deserialized_snappy);
    TEST_CHECK(root == htr);
}

// The beacon state blows up the stack to test on it
template <typename T>
void do_test_heap(const auto &case_dir) {
    auto node = YAML::LoadFile(case_dir.path().string() + value_file);
    auto decoded_ptr = std::make_unique<T>(node.template as<T>());
    auto ssz_snappy_path = case_dir.path().string() + ssz_file;
    std::ifstream ssz_snappy(ssz_snappy_path, std::ios::in | std::ios::binary);
    if (not ssz_snappy.is_open())
        throw std::filesystem::filesystem_error("could not open file", case_dir.path(), std::error_code());

    const auto ssz_snappy_size = std::filesystem::file_size(ssz_snappy_path);
    std::vector<std::byte> ssz_content(ssz_snappy_size);
    ssz_snappy.read(reinterpret_cast<char *>(ssz_content.data()), ssz_snappy_size);
    ssz_snappy.close();

    auto serialized_bytes = ssz::serialize(*decoded_ptr);
    std::vector<std::byte> ssz_output(serialized_bytes.size());
    if (!snappy::RawUncompress(reinterpret_cast<char *>(ssz_content.data()), ssz_snappy_size,
                               reinterpret_cast<char *>(ssz_output.data())))
        throw std::filesystem::filesystem_error("could not uncompress file", case_dir.path(), std::error_code());

    std::unique_ptr<T> deserialized_ptr(ssz::deserialize<T *>(serialized_bytes));
    TEST_CHECK(*deserialized_ptr == *decoded_ptr);
    auto deserialized_snappy_ptr = std::make_unique<T>(ssz::deserialize<T>(ssz_output));
    TEST_CHECK(*deserialized_snappy_ptr == *decoded_ptr);

    auto node_root = YAML::LoadFile(case_dir.path().string() + "/roots.yaml");
    auto root = node_root["root"].template as<ssz::chunk_t>();
    auto htr = ssz::hash_tree_root(*deserialized_snappy_ptr);
    TEST_CHECK(root == htr);
}

template <typename T>
void test_ssz_static(const std::string &&path, bool heap = false) {
    namespace fs = std::filesystem;
    for (auto &test_dir : fs::directory_iterator(base_path + path))
        for (auto &case_dir : fs::directory_iterator(test_dir)) {
            if (!heap) {
                do_test<T>(case_dir);
            } else {
                do_test_heap<T>(case_dir);
            }
        }
}

void test_merkleize() {
    hashtree_init(NULL);
    ssz::chunk_t one_hash{};
    ssz::hash_2_chunks(one_hash, ssz::zero_hash, ssz::zero_hash);
    TEST_CHECK(one_hash == ssz::zero_hash_array[1]);
    TEST_DUMP("Produced", reinterpret_cast<const char *>(one_hash.begin()), 32);

    auto log2 = helpers::log2ceil(1u);
    TEST_CHECK(log2 == 0);
    TEST_MSG("Obtained : %lu", log2);
    log2 = helpers::log2ceil(2u);
    TEST_CHECK(log2 == 1);
    TEST_MSG("Obtained : %lu", log2);
    log2 = helpers::log2ceil(3u);
    TEST_CHECK(log2 == 2);
    TEST_MSG("Obtained : %lu", log2);
    log2 = helpers::log2ceil(4u);
    TEST_CHECK(log2 == 2);
    TEST_MSG("Obtained : %lu", log2);
    log2 = helpers::log2ceil(5u);
    TEST_CHECK(log2 == 3);
    TEST_MSG("Obtained : %lu", log2);
    log2 = helpers::log2ceil(6u);
    TEST_CHECK(log2 == 3);
    TEST_MSG("Obtained : %lu", log2);

    auto htsize = ssz::_detail::compute_hashtree_size(1, 2);
    TEST_CHECK(htsize == 2 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

    htsize = ssz::_detail::compute_hashtree_size(1, 2);
    TEST_CHECK(htsize == 2 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

    htsize = ssz::_detail::compute_hashtree_size(2, 2);
    TEST_CHECK(htsize == 2 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

    htsize = ssz::_detail::compute_hashtree_size(3, 2);
    TEST_CHECK(htsize == 3 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

    htsize = ssz::_detail::compute_hashtree_size(4, 2);
    TEST_CHECK(htsize == 3 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

    htsize = ssz::_detail::compute_hashtree_size(4, 5);
    TEST_CHECK(htsize == 6 * ssz::BYTES_PER_CHUNK);
    TEST_MSG("Obtained : %lu", htsize);

}
const auto test_checkpoint = []() { test_ssz_static<ssz::checkpoint_t>("Checkpoint"); };
const auto test_attestation_data = []() { test_ssz_static<ssz::attestation_data_t>("AttestationData"); };
const auto test_attestation = []() { test_ssz_static<ssz::attestation_t>("Attestation"); };
const auto test_indexed_attestation = []() { test_ssz_static<ssz::indexed_attestation_t>("IndexedAttestation"); };
const auto test_aggregate_and_proof = []() { test_ssz_static<ssz::aggregate_and_proof_t>("AggregateAndProof"); };
const auto test_signed_aggregate_and_proof = []() {
    test_ssz_static<ssz::signed_aggregate_and_proof_t>("SignedAggregateAndProof");
};
const auto test_attester_slashing = []() { test_ssz_static<ssz::attester_slashing_t>("AttesterSlashing"); };
const auto test_bls_change = []() { test_ssz_static<ssz::bls_to_execution_change_t>("BLSToExecutionChange"); };
const auto test_signed_bls_change = []() {
    test_ssz_static<ssz::signed_bls_to_execution_change_t>("SignedBLSToExecutionChange");
};
const auto test_beacon_block_header = []() { test_ssz_static<ssz::beacon_block_header_t>("BeaconBlockHeader"); };
const auto test_signed_beacon_block_header = []() {
    test_ssz_static<ssz::signed_beacon_block_header_t>("SignedBeaconBlockHeader");
};
const auto test_eth1_data = []() { test_ssz_static<ssz::eth1_data_t>("Eth1Data"); };
const auto test_proposer_slashing = []() { test_ssz_static<ssz::proposer_slashing_t>("ProposerSlashing"); };
const auto test_deposit_data = []() { test_ssz_static<ssz::deposit_data_t>("DepositData"); };
const auto test_deposit_message = []() { test_ssz_static<ssz::deposit_message_t>("DepositMessage"); };
const auto test_deposit = []() { test_ssz_static<ssz::deposit_t>("Deposit"); };
const auto test_voluntary_exit = []() { test_ssz_static<ssz::voluntary_exit_t>("VoluntaryExit"); };
const auto test_signed_voluntary_exit = []() { test_ssz_static<ssz::signed_voluntary_exit_t>("SignedVoluntaryExit"); };
const auto test_sync_aggregate = []() { test_ssz_static<ssz::sync_aggregate_t>("SyncAggregate"); };
const auto test_withdrawal = []() { test_ssz_static<ssz::withdrawal_t>("Withdrawal"); };
const auto test_execution_payload = []() { test_ssz_static<ssz::execution_payload_t>("ExecutionPayload"); };
const auto test_execution_payload_header = []() {
    test_ssz_static<ssz::execution_payload_header_t>("ExecutionPayloadHeader");
};
const auto test_beacon_block_body = []() { test_ssz_static<ssz::beacon_block_body_t>("BeaconBlockBody"); };
const auto test_validator = []() { test_ssz_static<ssz::validator_t>("Validator"); };
const auto test_fork = []() { test_ssz_static<ssz::fork_t>("Fork"); };
const auto test_historical_summary = []() { test_ssz_static<ssz::historical_summary_t>("HistoricalSummary"); };
const auto test_beacon_state = []() { test_ssz_static<ssz::beacon_state_t>("BeaconState", true); };

TEST_LIST{{"checkpoint", test_checkpoint},
          {"attestation_data", test_attestation_data},
          {"attestation", test_attestation},
          {"indexed_attestation", test_indexed_attestation},
          {"aggregate_and_proof", test_aggregate_and_proof},
          {"signed_aggregate_and_proof", test_signed_aggregate_and_proof},
          {"attester_slashing", test_attester_slashing},
          {"bls_change", test_bls_change},
          {"signed_bls_change", test_signed_bls_change},
          {"beacon_block_header", test_beacon_block_header},
          {"signed_beacon_block_header", test_signed_beacon_block_header},
          {"eth1_data", test_eth1_data},
          {"proposer_slashing", test_proposer_slashing},
          {"deposit_data", test_deposit_data},
          {"deposit_message", test_deposit_message},
          {"deposit", test_deposit},
          {"voluntary_exit", test_voluntary_exit},
          {"signed_voluntary_exit", test_signed_voluntary_exit},
          {"sync_aggregate", test_sync_aggregate},
          {"withdrawal", test_withdrawal},
          {"execution_payload", test_execution_payload},
          {"execution_payload_header", test_execution_payload_header},
          {"beacon_block_body", test_beacon_block_body},
          {"validator", test_validator},
          {"fork", test_fork},
          {"historical_summary", test_historical_summary},
          {"beacon_state", test_beacon_state},
          {"merkleize", test_merkleize},
          {NULL, NULL}};
