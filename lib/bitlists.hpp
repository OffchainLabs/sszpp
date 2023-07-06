/*  bitlists.hpp
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


