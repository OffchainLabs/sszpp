/*  deposit.hpp
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
constexpr std::size_t DEPOSIT_CONTRACT_TREE_DEPTH{32};

struct deposit_data_t : ssz_container {
    bls_pubkey_t pubkey;
    Root withdrawal_credentials;
    uint64_t amount;
    signature_t signature;

    constexpr auto operator<=>(const deposit_data_t& rhs) const noexcept = default;
    constexpr bool operator==(const deposit_data_t& rhs) const noexcept = default;

    SSZ_CONT(pubkey, withdrawal_credentials, amount, signature);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, bls_pubkey_t&>("pubkey", pubkey),
              std::pair<const char*, Root&>("withdrawal_credentials", withdrawal_credentials),
              std::pair<const char*, uint64_t&>("amount", amount),
              std::pair<const char*, signature_t&>("signature", signature));
#endif
};

struct deposit_message_t : ssz_container {
    bls_pubkey_t pubkey;
    Root withdrawal_credentials;
    uint64_t amount;

    constexpr auto operator<=>(const deposit_message_t& rhs) const noexcept = default;
    constexpr bool operator==(const deposit_message_t& rhs) const noexcept = default;

    SSZ_CONT(pubkey, withdrawal_credentials, amount);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, bls_pubkey_t&>("pubkey", pubkey),
              std::pair<const char*, Root&>("withdrawal_credentials", withdrawal_credentials),
              std::pair<const char*, uint64_t&>("amount", amount));
#endif
};

struct deposit_t : ssz_container {
    std::array<Root, DEPOSIT_CONTRACT_TREE_DEPTH + 1> proof;
    deposit_data_t data;
    constexpr auto operator<=>(const deposit_t& rhs) const noexcept = default;
    constexpr bool operator==(const deposit_t& rhs) const noexcept = default;

    SSZ_CONT(proof, data);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, std::array<Root, DEPOSIT_CONTRACT_TREE_DEPTH + 1>&>("proof", proof),
              std::pair<const char*, deposit_data_t&>("data", data));
#endif
};
}  // namespace ssz
