/*  execution_payload.hpp
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
#include "withdrawals.hpp"
#include "uint256/uint256_t.h"

namespace ssz {
constexpr size_t MAX_BYTES_PER_TRANSACTION{1 << 30};
constexpr size_t MAX_TRANSACTIONS_PER_PAYLOAD{1 << 20};
constexpr size_t BYTES_PER_LOGS_BLOOM{256};
constexpr size_t MAX_EXTRA_DATA_BYTES{32};
constexpr size_t MAX_WITHDRAWALS_PER_PAYLOAD{16};

using transaction_t = ssz::list<std::byte, MAX_BYTES_PER_TRANSACTION>;

struct execution_payload_t : ssz_variable_size_container {
    Root parent_hash;
    execution_address_t fee_recipient;
    Root state_root, receipts_root;
    std::array<std::byte, BYTES_PER_LOGS_BLOOM> logs_bloom;
    Root prev_randao;
    uint64_t block_number, gas_limit, gas_used, timestamp;
    ssz::list<std::byte, MAX_EXTRA_DATA_BYTES> extra_data;
    uint256_t base_fee_per_gas;
    Root block_hash;
    ssz::list<transaction_t, MAX_TRANSACTIONS_PER_PAYLOAD> transactions;
    ssz::list<withdrawal_t, MAX_WITHDRAWALS_PER_PAYLOAD> withdrawals;

    constexpr auto operator<=>(const execution_payload_t& rhs) const noexcept = default;
    constexpr bool operator==(const execution_payload_t& rhs) const noexcept = default;

    SSZ_CONT(parent_hash, fee_recipient, state_root, receipts_root, logs_bloom, prev_randao, block_number, gas_limit,
             gas_used, timestamp, extra_data, base_fee_per_gas, block_hash, transactions, withdrawals);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Root&>("parent_hash", parent_hash),
              std::pair<const char*, execution_address_t&>("fee_recipient", fee_recipient),
              std::pair<const char*, Root&>("state_root", state_root),
              std::pair<const char*, Root&>("receipts_root", receipts_root),
              std::pair<const char*, std::array<std::byte, BYTES_PER_LOGS_BLOOM>&>("logs_bloom", logs_bloom),
              std::pair<const char*, Root&>("prev_randao", prev_randao),
              std::pair<const char*, uint64_t&>("block_number", block_number),
              std::pair<const char*, uint64_t&>("gas_limit", gas_limit),
              std::pair<const char*, uint64_t&>("gas_used", gas_used),
              std::pair<const char*, uint64_t&>("timestamp", timestamp),
              std::pair<const char*, ssz::list<std::byte, MAX_EXTRA_DATA_BYTES>&>("extra_data", extra_data),
              std::pair<const char*, uint256_t&>("base_fee_per_gas", base_fee_per_gas),
              std::pair<const char*, Root&>("block_hash", block_hash),
              std::pair<const char*, ssz::list<transaction_t, MAX_TRANSACTIONS_PER_PAYLOAD>&>("transactions",
                                                                                              transactions),
              std::pair<const char*, ssz::list<withdrawal_t, MAX_WITHDRAWALS_PER_PAYLOAD>&>("withdrawals",
                                                                                            withdrawals));
#endif
};

struct execution_payload_header_t : ssz_variable_size_container {
    Root parent_hash;
    execution_address_t fee_recipient;
    Root state_root, receipts_root;
    std::array<std::byte, BYTES_PER_LOGS_BLOOM> logs_bloom;
    Root prev_randao;
    uint64_t block_number, gas_limit, gas_used, timestamp;
    ssz::list<std::byte, MAX_EXTRA_DATA_BYTES> extra_data;
    uint256_t base_fee_per_gas;
    Root block_hash, transactions_root, withdrawals_root;

    constexpr auto operator<=>(const execution_payload_header_t& rhs) const noexcept = default;
    constexpr bool operator==(const execution_payload_header_t& rhs) const noexcept = default;

    SSZ_CONT(parent_hash, fee_recipient, state_root, receipts_root, logs_bloom, prev_randao, block_number, gas_limit,
             gas_used, timestamp, extra_data, base_fee_per_gas, block_hash, transactions_root, withdrawals_root);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, Root&>("parent_hash", parent_hash),
              std::pair<const char*, execution_address_t&>("fee_recipient", fee_recipient),
              std::pair<const char*, Root&>("state_root", state_root),
              std::pair<const char*, Root&>("receipts_root", receipts_root),
              std::pair<const char*, std::array<std::byte, BYTES_PER_LOGS_BLOOM>&>("logs_bloom", logs_bloom),
              std::pair<const char*, Root&>("prev_randao", prev_randao),
              std::pair<const char*, uint64_t&>("block_number", block_number),
              std::pair<const char*, uint64_t&>("gas_limit", gas_limit),
              std::pair<const char*, uint64_t&>("gas_used", gas_used),
              std::pair<const char*, uint64_t&>("timestamp", timestamp),
              std::pair<const char*, ssz::list<std::byte, MAX_EXTRA_DATA_BYTES>&>("extra_data", extra_data),
              std::pair<const char*, uint256_t&>("base_fee_per_gas", base_fee_per_gas),
              std::pair<const char*, Root&>("block_hash", block_hash),
              std::pair<const char*, Root&>("transactions_root", transactions_root),
              std::pair<const char*, Root&>("withdrawals_root", withdrawals_root));
#endif
};
}  // namespace ssz

#ifdef HAVE_YAML
// Yaml decoding
template <>
struct YAML::convert<uint256_t> {
    static bool decode(const YAML::Node& node, uint256_t& r) {
        std::string str;
        if (!YAML::convert<std::string>::decode(node, str)) return false;
        r = uint256_t(str, 10);
        return true;
    }
};
#endif
