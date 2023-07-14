/*  concepts.hpp
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
#include <bitset>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <uint256/uint256_t.h>

namespace ssz {
template <class T>
concept basic_type =
    std::is_same_v<std::remove_cvref_t<T>, std::uint8_t> || std::is_same_v<std::remove_cvref_t<T>, std::uint16_t> ||
    std::is_same_v<std::remove_cvref_t<T>, std::uint32_t> || std::is_same_v<std::remove_cvref_t<T>, std::uint64_t> ||
    std::is_same_v<std::remove_cvref_t<T>, bool> || std::is_same_v<std::remove_cvref_t<T>, std::byte> ||
    std::is_same_v<std::remove_cvref_t<T>, uint256_t>;

template <class R>
struct is_ssz_object : std::bool_constant<basic_type<R>> {};

template <std::ranges::sized_range R>
struct is_ssz_object<R> : is_ssz_object<std::ranges::range_value_t<R>> {};

template <std::size_t N>
struct is_ssz_object<std::bitset<N>> : std::true_type {};

template <class R>
concept serialized_range = std::ranges::input_range<R> && std::ranges::sized_range<R> &&
                           std::ranges::contiguous_range<R> && std::is_same_v<std::ranges::range_value_t<R>, std::byte>;

template <class R>
concept ssz_object = is_ssz_object<std::remove_cvref_t<R>>::value;

template <class R>
concept ssz_object_variable_size = ssz_object<R> && std::remove_cvref_t<R>::variable_size::value;

template <class R>
concept ssz_object_fixed_size = (ssz_object<R> && !ssz_object_variable_size<R>);

template <typename C>
struct is_ssz_array : std::bool_constant<std::is_array_v<C> && ssz_object<std::remove_pointer_t<std::decay_t<C>>> &&
                                         !std::is_same_v<std::decay_t<C>, bool *>> {};
template <ssz_object T, size_t N>
    requires(!std::is_same_v<std::remove_cvref_t<T>, bool>)
struct is_ssz_array<std::array<T, N>> : std::true_type {
    using item = T;
};

template <typename C>
struct is_ssz_vector : is_ssz_array<C> {};

template <ssz_object T, typename A>
    requires(!std::is_same_v<T, bool>)
struct is_ssz_vector<std::vector<T, A>> : std::true_type {
    using item = T;
};

template <typename C>
constexpr bool is_ssz_vector_v = is_ssz_vector<C>::value;
template <typename C>
constexpr bool is_ssz_array_v = is_ssz_array<C>::value;
template <typename C>
using ssz_vector_item_t = typename is_ssz_vector<C>::item;

template <typename C>
concept ssz_array = is_ssz_vector_v<C>;
template <typename C>
concept ssz_vector = is_ssz_vector_v<C>;
template <typename C>
concept ssz_fixed_sized_vector =
    ssz_vector<C> && (ssz_object_fixed_size<typename C::value_type> ||
                      (std::is_array_v<C> && ssz_object_fixed_size<std::remove_pointer_t<std::decay_t<C>>>));
template <typename C>
concept ssz_fixed_sized_array = ssz_fixed_sized_vector<C> && ssz_array<C>;
template <typename C>
concept ssz_basic_type_vector =
    ssz_vector<C> &&
    (basic_type<typename C::value_type> || (std::is_array_v<C> && basic_type<std::remove_pointer_t<std::decay_t<C>>>));
template <typename C>
concept ssz_basic_type_array = ssz_basic_type_vector<C> && ssz_array<C>;
}  // namespace ssz
