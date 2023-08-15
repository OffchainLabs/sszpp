/*  fork.hpp
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

#include "container.hpp"

namespace ssz {
struct fork_t : ssz_container {
    using version_t = std::array<std::byte, 4>;
    version_t  previous_version, current_version;
    Epoch epoch;

    constexpr auto operator<=>(const fork_t& rhs) const noexcept = default;
    constexpr bool operator==(const fork_t& rhs) const noexcept = default;

    SSZ_CONT(previous_version, current_version, epoch);
#ifdef HAVE_YAML
    YAML_CONT(std::pair<const char*, version_t&>("previous_version", previous_version),
              std::pair<const char*, version_t&>("current_version", current_version),
              std::pair<const char*, Epoch&>("epoch", epoch));
#endif
};
}
