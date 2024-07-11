/*  beaconchain.hpp
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

#include <cstdint>
namespace ssz {
using Slot = std::uint64_t;  // If you want a typesafe slot, use enum
using Epoch = std::uint64_t;
using ValidatorIndex = std::uint64_t;
using Root = std::array<std::byte, 32>;
using blob_kzg_commitments_t = std::array<std::byte, 48>;
using signature_t = std::array<std::byte, 96>;
using bls_pubkey_t = std::array<std::byte, 48>;
using execution_address_t = std::array<std::byte, 20>;
using Gwei = std::uint64_t;
}  // namespace ssz
