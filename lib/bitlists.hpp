/*  bitlists.hpp
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
#include "bytelists.hpp"

#ifdef HAVE_YAML
// specialization for bitlists
template <size_t N>
struct YAML::convert<ssz::list<bool, N>> {
    static bool decode(const YAML::Node &node, ssz::list<bool, N> &r) {
        std::string hexstring;
        if (!YAML::convert<std::string>::decode(node, hexstring)) return false;
        ssz::_sanitize_hextring(hexstring);
        if (hexstring.size() > 2 * N + 2) throw std::out_of_range("hex string larger than bytelist size");
        std::vector<std::byte> vec(hexstring.size() / 2 - 1);
        ssz::bytelist_from_str(hexstring, vec);
        ssz::deserialize(vec, r);
        return true;
    }
};

// specialization for bitsets
template <size_t N>
struct YAML::convert<std::bitset<N>> {
    static bool decode(const YAML::Node &node, std::bitset<N> &r) {
        std::string hexstring;
        if (!YAML::convert<std::string>::decode(node, hexstring)) return false;
        ssz::_sanitize_hextring(hexstring);
        if (hexstring.size() > 2 * N + 2) throw std::out_of_range("hex string larger than bytelist size");
        std::vector<std::byte> vec(hexstring.size() / 2 - 1);
        ssz::bytelist_from_str(hexstring, vec);
        ssz::deserialize(vec, r);
        return true;
    }
};
#endif


