/*  validator.hpp
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
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, bls_pubkey_t&>("pubkey", pubkey),
              std::pair<const char*, Root&>("withdrawal_credentials", withdrawal_credentials),
              std::pair<const char*, Gwei&>("effective_balance", effective_balance),
              std::pair<const char*, bool&>("slashed", slashed),
              std::pair<const char*, Epoch&>("activation_eligibility_epoch", activation_eligibility_epoch),
              std::pair<const char*, Epoch&>("activation_epoch", activation_epoch),
              std::pair<const char*, Epoch&>("exit_epoch", exit_epoch),
              std::pair<const char*, Epoch&>("withdrawable_epoch", withdrawable_epoch));
#endif
};
}
