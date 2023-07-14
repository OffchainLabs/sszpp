/*  container.hpp
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
#include <iterator>
#ifdef HAVE_YAML
#include <yaml-cpp/exceptions.h>
#include <yaml-cpp/yaml.h>
#endif

#include "lists.hpp"
#include "merkleize.hpp"

namespace ssz {

struct ssz_container {
    constexpr auto operator<=>(const ssz_container &rhs) const noexcept = default;
    constexpr bool operator==(const ssz_container &rhs) const noexcept = default;
};

struct ssz_variable_size_container : ssz_container {
    struct variable_size : std::true_type {};
};

template <class R>
    requires std::derived_from<R, ssz_container>
struct is_ssz_object<R> : std::true_type {};

// Serialization
#define SSZ_CONT(...)                                                                                              \
    constexpr std::size_t ssz_size() const noexcept { return ssz::compute_total_length(__VA_ARGS__); }             \
    constexpr void serialize(std::weakly_incrementable auto result) const { ssz::serialize(result, __VA_ARGS__); } \
    constexpr void deserialize(const std::ranges::sized_range auto &bytes) {                                       \
        ssz::deserialize_container(bytes, __VA_ARGS__);                                                            \
    }                                                                                                              \
    void hash_tree_root(std::weakly_incrementable auto result) const { ssz::hash_tree_root(result, __VA_ARGS__); }
#ifdef HAVE_YAML
#define YAML_CONT(...) \
    bool yaml_decode(const YAML::Node &node) { return ssz::yaml_decode_container(node, __VA_ARGS__); }
#endif
constexpr std::uint32_t size_or_placeholder(const ssz_object_fixed_size auto &member) { return ssz::size(member); }
constexpr std::uint32_t size_or_placeholder(const ssz_object_variable_size auto &member) {
    return BYTES_PER_LENGTH_OFFSET;
}

constexpr std::uint32_t size_plus_placeholder(const ssz_object_fixed_size auto &member) { return ssz::size(member); }
constexpr std::uint32_t size_plus_placeholder(const ssz_object_variable_size auto &member) {
    return ssz::size(member) + BYTES_PER_LENGTH_OFFSET;
}

constexpr std::uint32_t compute_fixed_length(const ssz_object auto &...members) {
    return (... + size_or_placeholder(members));
}

constexpr std::uint32_t compute_total_length(const ssz_object auto &...members) {
    return (... + size_plus_placeholder(members));
}

constexpr void serialize(std::weakly_incrementable auto result, const ssz_object auto &...members)
    requires std::is_same_v<decltype(*result), std::byte &>
{
    auto fsize = compute_fixed_length(members...);
    auto variable = result + fsize;
    auto begin = result;
    auto serialize_member = [&](const auto &member) {
        if constexpr (ssz_object_fixed_size<decltype(member)>) {
            serialize(result, member);
            result += ssz::size(member);
        } else {
            serialize(result, static_cast<std::uint32_t>(std::distance(begin, variable)));
            serialize(variable, member);
            result += BYTES_PER_LENGTH_OFFSET;
            variable += ssz::size(member);
        };
    };
    (serialize_member(members), ...);
}

template <class R>
    requires std::derived_from<R, ssz_container>
constexpr auto serialize(const R &container) {
    std::vector<std::byte> ret(container.ssz_size());
    serialize(ret.begin(), container);
    return ret;
}

template <class R>
    requires std::derived_from<R, ssz_container>
constexpr void serialize(std::weakly_incrementable auto result, const R &container) {
    return container.serialize(result);
}

// Deserialization
//
template <ssz_object R>
    requires std::derived_from<R, ssz_container>
auto deserialize(const serialized_range auto &bytes, R &r) {
    r.deserialize(bytes);
}

constexpr auto deserialize_fixed_size_members(const serialized_range auto &bytes, ssz_object auto &...members) {
    auto shifted_bytes = std::ranges::subrange(std::begin(bytes), std::end(bytes));
    std::vector<std::uint32_t> offsets{};
    auto deserialize_member = [&](auto &member) {
        if constexpr (ssz_object_fixed_size<decltype(member)>) {
            auto member_size = ssz::size(member);
            deserialize(shifted_bytes | std::views::take(member_size), member);
            shifted_bytes.advance(member_size);
        } else {
            offsets.push_back(*reinterpret_cast<const std::uint32_t *>(&*std::begin(shifted_bytes)));
            shifted_bytes.advance(BYTES_PER_LENGTH_OFFSET);
        }
    };
    (deserialize_member(members), ...);
    return offsets;
};

constexpr auto deserialize_variable_size_members(const serialized_range auto &bytes, const auto &offsets,
                                                 ssz_object auto &...members) {
    auto offset = offsets.begin();
    auto current_offset = *offset;
    auto deserialize_member = [&](auto &member) {
        if constexpr (ssz_object_fixed_size<decltype(member)>) return;
        offset++;
        if (offset != std::end(offsets)) {
            auto next_offset = *offset;
            deserialize(std::ranges::subrange(std::begin(bytes) + current_offset, std::begin(bytes) + next_offset),
                        member);
            current_offset = next_offset;
        } else {
            deserialize(bytes | std::views::drop(current_offset), member);
        }
    };
    (deserialize_member(members), ...);
}

constexpr void deserialize_container(const serialized_range auto &bytes, ssz_object auto &...members) {
    auto offsets = deserialize_fixed_size_members(bytes, members...);
    if (offsets.empty()) {
        return;
    }
    deserialize_variable_size_members(bytes, offsets, members...);
}

#ifdef HAVE_YAML
// YAML
template <typename R>
struct is_yaml_pair : std::false_type {};

template <ssz_object T>
struct is_yaml_pair<std::pair<const char *, T &>> : std::true_type {};

template <typename R>
concept yaml_pair = is_yaml_pair<R>::value;

namespace _detail {
auto _decode_member = [](const YAML::Node &node, yaml_pair auto pair) {
    return YAML::convert<std::remove_reference_t<decltype(pair.second)>>::decode(node[pair.first], pair.second);
};
}

bool yaml_decode_container(const YAML::Node &node, yaml_pair auto... pairs) {
    return (_detail::_decode_member(node, pairs), ...);
}
#endif

// hash_tree_root of containers
template <std::weakly_incrementable I, class R>
    requires std::derived_from<R, ssz::ssz_container>
void hash_tree_root(I result, const R &r) {
    r.hash_tree_root(result);
}

template <class R>
    requires std::derived_from<R, ssz::ssz_container>
auto hash_tree_root(const R &r) {
    chunk_t ret{};
    r.hash_tree_root(std::begin(ret));
    return ret;
}

void hash_tree_root(std::weakly_incrementable auto result, const ssz_object auto &...members)
    requires std::is_same_v<decltype(*result), std::byte &>
{
    std::vector<std::byte> ret(sizeof...(members) * BYTES_PER_CHUNK);
    auto to_hash = std::begin(ret);
    auto htr_member = [&](const auto &member) {
        ssz::hash_tree_root(to_hash, member);
        to_hash += BYTES_PER_CHUNK;
    };
    (htr_member(members), ...);
    hash_tree_root(result, ret);
}
}  // namespace ssz

#ifdef HAVE_YAML
template <class R>
    requires std::derived_from<R, ssz::ssz_container>
struct YAML::convert<R> {
    static YAML::Node encode(const R &r) { return yaml_encode(r); }
    static bool decode(const YAML::Node &node, R &r) { return r.yaml_decode(node); }
};
#endif
