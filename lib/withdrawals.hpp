/*  withdrawals.hpp
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
#include "beaconchain.hpp"

namespace ssz {

struct bls_to_execution_change_t : ssz_container {
    ValidatorIndex validator_index;
    bls_pubkey_t from_bls_pubkey;
    execution_address_t to_execution_address;

    constexpr auto operator<=>(const bls_to_execution_change_t& rhs) const noexcept = default;
    constexpr bool operator==(const bls_to_execution_change_t& rhs) const noexcept = default;

    SSZ_CONT(validator_index, from_bls_pubkey, to_execution_address);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, ValidatorIndex&>("validator_index", validator_index),
              std::pair<const char*, bls_pubkey_t&>("from_bls_pubkey", from_bls_pubkey),
              std::pair<const char*, execution_address_t&>("to_execution_address", to_execution_address));
#endif
};

struct signed_bls_to_execution_change_t : ssz_container {
    bls_to_execution_change_t message;
    signature_t signature;

    constexpr auto operator<=>(const signed_bls_to_execution_change_t& rhs) const noexcept = default;
    constexpr bool operator==(const signed_bls_to_execution_change_t& rhs) const noexcept = default;

    SSZ_CONT(message, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, bls_to_execution_change_t&>("message", message),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

using WithdrawalIndex = std::uint64_t;

struct withdrawal_t : ssz_container {
    WithdrawalIndex index;
    ValidatorIndex validator_index;
    execution_address_t address;
    Gwei amount;

    constexpr auto operator<=>(const withdrawal_t& rhs) const noexcept = default;
    constexpr bool operator==(const withdrawal_t& rhs) const noexcept = default;

    SSZ_CONT(index, validator_index, address, amount);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, WithdrawalIndex&>("index", index),
              std::pair<const char*, ValidatorIndex&>("validator_index", validator_index),
              std::pair<const char*, execution_address_t&>("address", address),
              std::pair<const char*, Gwei&>("amount", amount));
#endif
};

}  // namespace ssz
