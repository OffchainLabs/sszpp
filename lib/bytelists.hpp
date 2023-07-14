/*  bytelists.hpp
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
#include "lists.hpp"
#ifdef HAVE_YAML
#include <yaml-cpp/yaml.h>
#endif

namespace ssz {
namespace _detail{
struct Table {
    std::array<std::byte, 128> tab;
    constexpr Table() : tab{} {
        tab['1'] = std::byte{1};
        tab['2'] = std::byte{2};
        tab['3'] = std::byte{3};
        tab['4'] = std::byte{4};
        tab['5'] = std::byte{5};
        tab['6'] = std::byte{6};
        tab['7'] = std::byte{7};
        tab['8'] = std::byte{8};
        tab['9'] = std::byte{9};
        tab['a'] = std::byte{10};
        tab['A'] = std::byte{10};
        tab['b'] = std::byte{11};
        tab['B'] = std::byte{11};
        tab['c'] = std::byte{12};
        tab['C'] = std::byte{12};
        tab['d'] = std::byte{13};
        tab['D'] = std::byte{13};
        tab['e'] = std::byte{14};
        tab['E'] = std::byte{14};
        tab['f'] = std::byte{15};
        tab['F'] = std::byte{15};
    }
    constexpr auto operator[](char const idx) const { return tab[(std::size_t)idx]; }
} constexpr table;

constexpr auto hex_to_byte(char number) { return table[(std::size_t)number]; }
}  // namespace _detail

constexpr auto _sanitize_hextring(const std::string_view &str) {
    if (!str.starts_with("0x")) throw std::invalid_argument("string not prepended with 0x");
    if (str.size() & 1) throw std::invalid_argument("not an even number of characters {}");
}

constexpr auto bytelist_from_str(const std::string_view &str, std::ranges::sized_range auto &r) {
    auto N = std::ranges::size(r);
    _sanitize_hextring(str);
    if (str.size() > 2 * N + 2) throw std::out_of_range("hex string larger than bytelist size");

    std::ranges::transform(str | std::views::drop(2) | std::views::chunk(2), std::begin(r), [](const auto &chunk) {
        return _detail::hex_to_byte(chunk[0]) << 4 | _detail::hex_to_byte(chunk[1]);
    });
}
}  // namespace ssz
#ifdef HAVE_YAML
// YAML encoding/decoding to hextrings for bytevectors.
template <std::ranges::sized_range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, std::byte>
struct YAML::convert<R> {
    static bool decode(const YAML::Node &node, R &r) {
        std::string hexstring;
        if (!YAML::convert<std::string>::decode(node, hexstring)) return false;
        ssz::bytelist_from_str(hexstring, r);
        return true;
    }
};

template <size_t N>
struct YAML::convert<ssz::list<std::byte, N>> {
    static bool decode(const YAML::Node &node, ssz::list<std::byte, N> &r) {
        std::string hexstring;
        if (!YAML::convert<std::string>::decode(node, hexstring)) return false;
        if (hexstring.size() <= 2) return true;
        std::vector<std::byte> vec(hexstring.size() / 2 - 1);
        ssz::bytelist_from_str(hexstring, vec);
        r.reset(vec);
        return true;
    }
};

// need to explicitly override yaml-cpp's implementation for arrays
template <std::size_t N>
struct YAML::convert<std::array<std::byte, N>> {
    static bool decode(const YAML::Node &node, std::array<std::byte, N> &r) {
        std::string hexstring;
        if (!YAML::convert<std::string>::decode(node, hexstring)) return false;
        ssz::bytelist_from_str(hexstring, r);
        return true;
    }
};
#endif
