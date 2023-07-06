/*  validator.hpp
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

namespace ssz {
struct validator_t : ssz_container {
    bls_pubkey_t pubkey;
    Root withdrawal_credentials;
    Gwei effective_balance;
    bool slashed;
    Epoch activation_eligibility_epoch, activation_epoch, exit_epoch, withdrawable_epoch;

    constexpr auto operator<=>(const validator_t& rhs) const noexcept = default;
    constexpr bool operator==(const validator_t& rhs) const noexcept = default;

    SSZ_CONT(pubkey, withdrawal_credentials, effective_balance, slashed, activation_eligibility_epoch, activation_epoch,
             exit_epoch, withdrawable_epoch);
    YAML_CONT(std::pair<const char*, bls_pubkey_t&>("pubkey", pubkey),
              std::pair<const char*, Root&>("withdrawal_credentials", withdrawal_credentials),
              std::pair<const char*, Gwei&>("effective_balance", effective_balance),
              std::pair<const char*, bool&>("slashed", slashed),
              std::pair<const char*, Epoch&>("activation_eligibility_epoch", activation_eligibility_epoch),
              std::pair<const char*, Epoch&>("activation_epoch", activation_epoch),
              std::pair<const char*, Epoch&>("exit_epoch", exit_epoch),
              std::pair<const char*, Epoch&>("withdrawable_epoch", withdrawable_epoch));
};
}
