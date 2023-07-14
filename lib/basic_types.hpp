/*  basic_types.hpp
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
#include "concepts.hpp"

namespace ssz {
// Type traits
constexpr size_t size(basic_type auto i) noexcept { return sizeof(i); }

// Serialization
constexpr void serialize(std::weakly_incrementable auto result, const basic_type auto i)
    requires std::is_same_v<decltype(*result), std::byte &>
{
    if constexpr (std::endian::native == std::endian::big) {
        i = std::byteswap(i);
    }
    std::copy(static_cast<const std::byte *>(static_cast<const void *>(&i)),
              static_cast<const std::byte *>(static_cast<const void *>(&i)) + sizeof(i), result);
}
constexpr auto serialize(const basic_type auto i) {
    std::vector<std::byte> ret(sizeof(i));
    serialize(ret.begin(), i);
    return ret;
}

// Deserialization
template <basic_type T>
constexpr void deserialize(const serialized_range auto &bytes, T &ret) {
    if (bytes.size() < sizeof(ret)) {
        throw std::invalid_argument("not enough serialized bytes");
    }
    std::copy(static_cast<const T *>(static_cast<const void *>(&*bytes.begin())),
              static_cast<const T *>(static_cast<const void *>(&*bytes.begin())) + 1, &ret);
    if constexpr (std::endian::native == std::endian::big) {
        ret = std::byteswap(ret);
    }
}
constexpr void deserialize(const serialized_range auto &bytes, bool &ret) {
    if (bytes.size() == 0) {
        throw std::invalid_argument("not enough serialized bytes");
    }
    if (*bytes.begin() == std::byte{0x00}) {
        ret = false;
    } else if (*bytes.begin() == std::byte{0x01}) {
        ret = true;
    } else {
        throw std::invalid_argument("not a valid serialization of bool");
    }
}
}  // namespace ssz
