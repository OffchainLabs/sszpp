/*  ssz++.hpp
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

#include <stdexcept>
#include "bitlists.hpp"
#include "container.hpp"

namespace ssz {
template <ssz_object T>
    requires(!std::is_pointer_v<T>)
T deserialize(const serialized_range auto &bytes) {
    T ret{};
    deserialize(bytes, ret);
    return ret;
}

template <typename T>
    requires(std::is_pointer_v<T> && ssz_object<std::remove_pointer_t<T>>)
T deserialize(const serialized_range auto &bytes) {
    auto ret = new (std::remove_pointer_t<T>);
    deserialize(bytes, *ret);
    return ret;
}
} // namespace ssz
