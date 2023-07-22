/*  merkleize.hpp
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

#include <hashtree.h>
#include <algorithm>  //copy
#include <iterator>
#include <type_traits>
#include <future>

#include "basic_types.hpp"
#include "lists.hpp"
#include "math.hpp"
#include "beaconchain.hpp"

namespace ssz {
constexpr std::size_t BYTES_PER_CHUNK{32};
constexpr auto zero_hash_depth{42u};
using chunk_t = std::array<std::byte, ssz::BYTES_PER_CHUNK>;
constexpr chunk_t zero_hash{};

/**
 * \brief wrapper to hashtree's hash function
 */
auto hash(std::weakly_incrementable auto output, const std::weakly_incrementable auto& input, std::uint64_t count)
    requires(std::is_same_v<decltype(*output), std::byte&> &&
             std::is_same_v<std::remove_cvref_t<decltype(*input)>, std::byte>)
{
    hashtree_hash(reinterpret_cast<unsigned char*>(&*output), reinterpret_cast<const unsigned char*>(&*input), count);
    return output + count * BYTES_PER_CHUNK;
}

auto hash(std::weakly_incrementable auto output, const serialized_range auto& input, uint64_t count = 0)
    requires std::is_same_v<decltype(*output), std::byte&>
{
    if (count == 0) count = std::ranges::size(input) / 2 / BYTES_PER_CHUNK;
    return hash(output, std::begin(input), count);
}

auto hash(serialized_range auto& output, const serialized_range auto& input, uint64_t count = 0) {
    if (count == 0) count = std::ranges::size(input) / 2 / BYTES_PER_CHUNK;
    return hash(std::begin(output), std::begin(input), count);
}

/**
 * \brief helper to hash two non-necessarily consecutive chunks of 32 bytes.
 *
 * does not check bounds, undefined behavior if the two ranges do not have at least 32 bytes
 */
auto hash_2_chunks(std::weakly_incrementable auto output, const std::weakly_incrementable auto& first,
                   const std::weakly_incrementable auto& second)
    requires(std::is_same_v<std::remove_cvref_t<decltype(*output)>, std::byte> &&
             std::is_same_v<std::remove_cvref_t<decltype(*first)>, std::byte> &&
             std::is_same_v<std::remove_cvref_t<decltype(*second)>, std::byte>)
{
    std::array<std::byte, 2 * BYTES_PER_CHUNK> sum;
    std::ranges::copy(first, first + BYTES_PER_CHUNK, std::begin(sum));
    std::ranges::copy(second, second + BYTES_PER_CHUNK, std::begin(sum) + BYTES_PER_CHUNK);
    return hash(output, sum, 1);
}

auto hash_2_chunks(std::weakly_incrementable auto output, const serialized_range auto& first,
                   const serialized_range auto& second)
    requires(std::is_same_v<std::remove_cvref_t<decltype(*output)>, std::byte>)
{
    return hash_2_chunks(output, std::begin(first), std::begin(second));
}

auto hash_2_chunks(serialized_range auto& output, const serialized_range auto& first,
                   const serialized_range auto& second) {
    return hash_2_chunks(std::begin(output), std::begin(first), std::begin(second));
}

auto hash_2_chunks(std::weakly_incrementable auto output, const std::weakly_incrementable auto& first,
                   const serialized_range auto& second)
    requires(std::is_same_v<std::remove_cvref_t<decltype(*output)>, std::byte> &&
             std::is_same_v<std::remove_cvref_t<decltype(*first)>, std::byte>)
{
    return hash_2_chunks(output, first, std::begin(second));
}

namespace _detail {
template <std::size_t N>
auto zero_hash_array_helper() {
    std::array<chunk_t, N> ret;
    ret[0] = zero_hash;
    hashtree_init(NULL);
    for (std::size_t i = 1; i < N; i++) ssz::hash_2_chunks(ret[i], ret[i - 1], ret[i - 1]);
    return ret;
}
}  // namespace _detail

const auto zero_hash_array = _detail::zero_hash_array_helper<zero_hash_depth>();

namespace _detail {
/**
 * \brief comutes the minimal size in bytes required to hold the hashtree for the given buffer
 *
 * It is faster to loop over computing the actual size than overestimating using logarithms and allocating unnecessary
 * memory
 */
auto compute_hashtree_size(std::size_t chunk_count, std::size_t depth) {
    std::size_t ret{};
    if (depth == 0) return BYTES_PER_CHUNK;
    while (depth > 0) {
        if (chunk_count & 1) chunk_count++;
        ret += chunk_count >>= 1;
        depth--;
    }
    return ret * BYTES_PER_CHUNK;
}

/**
 * \brief helper to hash the first layer without copying
 *
 * Undefined behaviour if one chunk or less is sent.
 */
auto hash_first_layer(std::weakly_incrementable auto& hash_tree, const std::weakly_incrementable auto& chunks,
                      size_t byte_length)
    requires std::is_same_v<std::remove_cvref_t<decltype(*chunks)>, std::byte>
{
    auto chunk_count = (byte_length + BYTES_PER_CHUNK - 1) / BYTES_PER_CHUNK;
    // if there are left over bytes we can't hash in-place
    auto left_over_bytes = (byte_length % BYTES_PER_CHUNK) != 0;
    if (!(chunk_count & 1) && !left_over_bytes) {
        return hash(hash_tree, chunks, chunk_count / 2);
    }
    // We had some left over bytes or an odd number of chunks hash as much as we can without copying
    auto first_blocks = (chunk_count - 1) / 2;
    if (first_blocks) {
        hash_tree = hash(hash_tree, chunks, first_blocks);
    }

    // hash the last two chunks copying them
    auto first_blocks_size = first_blocks * 2 * BYTES_PER_CHUNK;
    std::array<std::byte, 2 * BYTES_PER_CHUNK> last_chunk{};
    std::ranges::copy_n(chunks + first_blocks_size, byte_length - first_blocks_size, std::begin(last_chunk));
    return hash(hash_tree, last_chunk, 1);
}
}

/**
 * \brief implements merkleize(chunks, limit=None) in
 * https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 * \param[in] hash_tree, a pre-allocated buffer where the hash_three will be written
 * \param[in] chunks a range of bytes that contains the chunks to be merkleized. They are not assumed to be chunked,
 * that is, this function will add the extra zero bytes to pad the last chunk if necessary.
 * \param[in] limit the exponential of the tree depth
 */
void sparse_hash_tree(std::weakly_incrementable auto hash_tree, const std::weakly_incrementable auto& chunks,
                      size_t byte_length, std::size_t depth)
    requires(std::is_same_v<std::remove_cvref_t<decltype(*hash_tree)>, std::byte> &&
             std::is_same_v<std::remove_cvref_t<decltype(*chunks)>, std::byte>)
{
    if (depth == 0) {
        std::ranges::copy_n(chunks, byte_length, hash_tree);
        return;
    }
    auto first = hash_tree;
    auto last = _detail::hash_first_layer(hash_tree, chunks, byte_length);
    for (std::size_t height = 1; height < depth; height++) {
        auto dist = std::distance(first, last) / BYTES_PER_CHUNK;
        auto next_first = last;
        if (dist > 1) last = hash(last, first, dist / 2);
        if (dist & 1) last = hash_2_chunks(last, next_first - BYTES_PER_CHUNK, zero_hash_array[height]);
        first = next_first;
    }
}

/**
 * \brief implements merkleize(chunks, limit=None) in
 * https://github.com/ethereum/consensus-specs/blob/dev/ssz/simple-serialize.md
 *
 * \param[in] chunks an iterator to range of bytes that contains the chunks to be merkleized. They are not assumed to be
 * chunked, that is, this function will add the extra zero bytes to pad the last chunk if necessary.
 * \param[in] byte_length, the number of bytes to hash (the size in bytes of the memory buffer pointed to by chunks).
 * \param[in] limit the exponential of the tree depth.
 */
auto sparse_hash_tree(const std::weakly_incrementable auto& chunks, std::size_t byte_length, std::size_t limit = 0)
    requires std::is_same_v<std::remove_cvref_t<decltype(*chunks)>, std::byte>
{
    auto chunk_count = (byte_length + ssz::BYTES_PER_CHUNK - 1) / ssz::BYTES_PER_CHUNK;
    auto depth = (limit == 0) ? helpers::log2ceil(chunk_count) : helpers::log2ceil(limit);
    if (chunk_count == 0) {
        std::vector<std::byte> ret(BYTES_PER_CHUNK);
        std::ranges::copy(zero_hash_array[depth], std::back_inserter(ret));
        return ret;
    }
    std::vector<std::byte> ret(_detail::compute_hashtree_size(chunk_count, depth));
    sparse_hash_tree(std::begin(ret), chunks, byte_length, depth);
    return ret;
}

/**
 * \brief merkleize basic type vectors
 *
 * This function recasts the passed vector to a vector of bytes, the container has to be a contiguous range in memory
 */
auto sparse_hash_tree(const ssz_basic_type_vector auto& chunks, std::size_t limit = 0) {
    return sparse_hash_tree(reinterpret_cast<const std::byte*>(&*std::begin(chunks)), ssz::size(chunks), limit);
}

// helper to mix in length in place
auto mix_in_length(std::weakly_incrementable auto output, const std::weakly_incrementable auto& hash,
                   std::uint64_t length)
    requires(std::is_same_v<std::remove_cvref_t<decltype(*output)>, std::byte> &&
             std::is_same_v<std::remove_cvref_t<decltype(*hash)>, std::byte>)
{
    chunk_t serialized_length{};
    ssz::serialize(std::begin(serialized_length), length);
    hash_2_chunks(output, hash, serialized_length);
}

// hash_tree_root of basic objects
auto hash_tree_root(std::weakly_incrementable auto output, ssz::basic_type auto n) { serialize(output, n); }
auto hash_tree_root(ssz::basic_type auto n) {
    chunk_t ret{};
    serialize(std::begin(ret), n);
    return ret;
}

// hash_tree_root of vectors of basic objects, no copy on little endian systems
void _htr_little_endian_basic_list(auto result, const auto& r, size_t limit = 0) {
    auto hash_tree = sparse_hash_tree(r, limit);
    std::ranges::copy(std::ranges::subrange(std::end(hash_tree) - BYTES_PER_CHUNK, std::end(hash_tree)), result);
}

void _htr_big_endian_basic_list(auto result, const auto& r, size_t limit = 0) {
    auto serialized = serialize(r);
    _htr_little_endian_basic_list(result, serialized, limit);
}
template <std::ranges::sized_range R>
auto hash_tree_root(std::weakly_incrementable auto result, const R& r, size_t limit = 0)
    requires std::is_same_v<decltype(*result), std::byte&> &&
             basic_type<std::ranges::range_value_t<R>> && std::ranges::contiguous_range<R>
{
    if constexpr (std::endian::native == std::endian::little) {
        _htr_little_endian_basic_list(result, r, limit);
    } else {
        _htr_big_endian_basic_list(result, r, limit);
    }
}
template <std::ranges::sized_range R>
auto hash_tree_root(const R& r, size_t limit = 0)
    requires basic_type<std::ranges::range_value_t<R>> && std::ranges::contiguous_range<R>
{
    chunk_t ret{};
    hash_tree_root(std::begin(ret), r, limit);
    return ret;
}

// hash_tree_root of array/list of roots, avoid an extra copy and allocation
auto _htr_array_of_array_little_endian(auto result, const auto& r, size_t limit = 0) {
    auto hash_tree = sparse_hash_tree(reinterpret_cast<const std::byte*>(&*std::begin(r)),
                                      std::ranges::size(r) * BYTES_PER_CHUNK, limit);
    std::ranges::copy_n(std::end(hash_tree) - BYTES_PER_CHUNK, BYTES_PER_CHUNK, result);
}

template <std::ranges::sized_range R>
    requires std::is_same_v<Root, std::remove_cvref_t<std::ranges::range_value_t<R>>>
auto hash_tree_root(std::weakly_incrementable auto result, const R& r, size_t limit = 0) {
    if constexpr (std::endian::native == std::endian::little) {
        _htr_array_of_array_little_endian(result, r, limit);
    } else {
        _htr_big_endian_basic_list(result, r, limit);
    }
}

// helper hash_tree_root of non-basic, 32 bytes, or boolean vectors
template <ssz_vector R>
    requires(!std::is_same_v<Root, std::remove_cvref_t<std::ranges::range_value_t<R>>> && !ssz_basic_type_vector<R>)
auto hash_tree_root(std::weakly_incrementable auto result, const R& r, size_t limit = 0) {
    auto rsize = std::ranges::size(r);
    if ((rsize < (1ul << 15))) {
        std::vector<std::byte> chunks(std::ranges::size(r) * BYTES_PER_CHUNK);
        auto offset = std::begin(chunks);
        std::ranges::for_each(r, [&offset](auto& elem) {
            hash_tree_root(offset, elem);
            std::advance(offset, BYTES_PER_CHUNK);
        });
        return hash_tree_root(result, chunks, limit);
    }
    auto half_size = std::bit_ceil(rsize) / 2;
    auto first = std::ranges::subrange(std::begin(r), std::begin(r) + half_size);
    auto last = std::ranges::subrange(std::begin(r) + half_size, std::end(r));
    std::vector<std::byte> two_blocks(2 * BYTES_PER_CHUNK);  // has to be on the heap
    auto future = std::async(std::launch::async,
                             [&]() { hash_tree_root(std::begin(two_blocks) + BYTES_PER_CHUNK, last, half_size); });
    hash_tree_root(std::begin(two_blocks), first, half_size);
    future.get();
    hash(result, two_blocks, 1);
    for (auto i = helpers::log2ceil(half_size) + 1; i < helpers::log2ceil(limit); i++) {
        hash_2_chunks(result, result, zero_hash_array[i]);
    }
}

// hash_tree_root of ssz::list of basic type
template <basic_type T, size_t N>
auto hash_tree_root(std::weakly_incrementable auto result, const ssz::list<T, N>& r) {
    size_t limit = (N * sizeof(T) + BYTES_PER_CHUNK - 1) / BYTES_PER_CHUNK;
    auto hash = hash_tree_root(r.data(), limit);
    mix_in_length(result, std::begin(hash), r.size());
}

template <ssz_object T>
    requires(!basic_type<T>)
auto hash_tree_root(const std::vector<T>& r, size_t limit = 0) {
    chunk_t hash{};
    hash_tree_root(std::begin(hash), r, limit);
    return hash;
}

template <ssz_object T, size_t N>
    requires(!basic_type<T>)
auto hash_tree_root(std::weakly_incrementable auto result, const ssz::list<T, N>& r) {
    auto hash = hash_tree_root(r.data(), N);
    return mix_in_length(result, std::begin(hash), r.size());
}

template <ssz_object T, size_t N>
    requires(!basic_type<T>)
auto hash_tree_root(const ssz::list<T, N>& r) {
    chunk_t hash{};
    hash_tree_root(std::begin(hash), r);
    return hash;
}

/**
 *  \brief hash_tree_root of ssz::list<bool, N>
 *
 */
template <size_t N>
auto hash_tree_root(std::weakly_incrementable auto result, const ssz::list<bool, N>& r)
    requires std::is_same_v<decltype(*result), std::byte&>
{
    auto bytes = serialize(r);
    // remove the last bit on the list
    auto& last = bytes.back();
    auto highest_bit = CHAR_BIT - std::countl_zero(static_cast<unsigned char>(last)) - 1;
    if (highest_bit == 0) {
        bytes.pop_back();
    } else {
        last &= ~(std::byte{1} << highest_bit);
    }
    size_t limit = (N + CHAR_BIT * BYTES_PER_CHUNK - 1) / (CHAR_BIT * BYTES_PER_CHUNK);
    auto hash = hash_tree_root(bytes, limit);
    mix_in_length(result, std::begin(hash), r.size());
}

/**
 * \brief hash_tree_root of std::bitset<N>
 */
template <size_t N>
auto hash_tree_root(std::weakly_incrementable auto result, const std::bitset<N>& r)
    requires std::is_same_v<std::remove_cvref_t<decltype(*result)>, std::byte>
{
    auto bytes = serialize(r);
    size_t limit = (N + CHAR_BIT * BYTES_PER_CHUNK - 1) / (CHAR_BIT * BYTES_PER_CHUNK);
    hash_tree_root(result, bytes, limit);
}

}  // namespace ssz
