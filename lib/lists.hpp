/*  lists.hpp
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
#include <algorithm>
#include <array>
#include <climits>
#ifdef HAVE_YAML
#include <yaml-cpp/yaml.h>
#endif

#include "basic_types.hpp"

namespace ssz {
const uint32_t BYTES_PER_LENGTH_OFFSET{4};

template <ssz_object T, std::size_t N>
class list {
   private:
    std::vector<T> m_list;

   public:
    list(const std::vector<T> &list = {}) noexcept : m_list{list} {};
    list(std::vector<T> &&list) noexcept : m_list{std::move(list)} {};

    constexpr auto begin() noexcept { return m_list.begin(); }
    constexpr auto begin() const noexcept { return m_list.begin(); }
    constexpr auto rbegin() noexcept { return m_list.rbegin(); }
    constexpr auto cbegin() const noexcept { return m_list.cbegin(); }
    constexpr auto crbegin() const noexcept { return m_list.crbegin(); }
    constexpr auto end() noexcept { return m_list.end(); }
    constexpr auto end() const noexcept { return m_list.end(); }
    constexpr auto rend() noexcept { return m_list.rend(); }
    constexpr auto cend() const noexcept { return m_list.cend(); }
    constexpr auto crend() const noexcept { return m_list.crend(); }
    constexpr auto size() const noexcept { return m_list.size(); }
    static constexpr auto limit() noexcept { return N; }
    constexpr auto reset(std::vector<T> &vec) noexcept { m_list = std::move(vec); }
    constexpr void push_back(T &&value) { m_list.push_back(std::move(value)); }
    constexpr void push_back(const T &value) { m_list.push_back(value); }
    auto &data() noexcept { return m_list; }
    constexpr auto &data() const noexcept { return m_list; }

    struct variable_size : std::true_type {};
    using value_type = typename std::vector<T>::value_type;
    using allocator_type = typename std::vector<T>::allocator_type;
    using size_type = typename std::vector<T>::size_type;
    using difference_type = typename std::vector<T>::difference_type;
    using referece = typename std::vector<T>::reference;
    using const_referece = typename std::vector<T>::const_reference;
    using pointer = typename std::vector<T>::pointer;
    using const_pointer = typename std::vector<T>::const_pointer;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

    constexpr auto &operator[](size_type pos) { return m_list[pos]; }
    constexpr auto &operator[](size_type pos) const { return m_list[pos]; }

    constexpr auto operator<=>(const list<T, N> &rhs) const noexcept = default;
    constexpr bool operator==(const list<T, N> &rhs) const noexcept = default;
};

// Type traits

// forward declaration
struct ssz_container;
template <class R>
    requires std::derived_from<R, ssz_container>
constexpr size_t size(const R &container) noexcept {
    return container.ssz_size();
}

// The size of a list with fixed sized objects (not bool)
template <std::ranges::sized_range R>
constexpr size_t size(const R &r) noexcept
  requires ssz_object_fixed_size<std::ranges::range_value_t<R>>
{
  auto rsize = std::ranges::size(r);
  return (rsize == 0) ? 0 : rsize * ssz::size(*std::begin(r));
}
// The size of a list with variable sized objects
template <std::ranges::sized_range R>
constexpr size_t size(const R &r) noexcept
    requires ssz_object_variable_size<std::ranges::range_value_t<R>>
{
  return std::ranges::fold_left(r, 0ul, [](size_t n, const ssz_object_variable_size auto &m) {
      return n + BYTES_PER_LENGTH_OFFSET + ssz::size(m);
  });
}
// The size of a bitvector modelled as a bitset
template <std::size_t N>
constexpr size_t size(const std::bitset<N> &r) noexcept {
  return (N + CHAR_BIT - 1) / CHAR_BIT;
}
// The size of a bitvector modelled as a std::vector<bool>
constexpr size_t size(const std::vector<bool> &r) noexcept {
  return (r.size() + CHAR_BIT - 1) / CHAR_BIT;
}
// The size of a bitlist
template <std::size_t N>
constexpr size_t size(const ssz::list<bool, N> &r) noexcept {
  return r.size() / CHAR_BIT + 1;
}
// Serialization
namespace {
// contiguous lists of basic types (use casting and one memory copy)
constexpr void _serialize_little_endian_basic_list(auto result, const auto &r) {
  std::copy(static_cast<const std::byte *>(
                static_cast<const void *>(&*std::begin(r))),
            static_cast<const std::byte *>(
                static_cast<const void *>(&*std::begin(r))) +
                size(*std::begin(r)) * std::size(r),
            result);
}
// contiguous lists of basic types on big endian systems
constexpr void _serialize_big_endian_basic_list(auto result, const auto &r) {
  decltype(r) copied{};
  std::ranges::transform(r, copied, std::byteswap);
  std::copy(static_cast<const std::byte *>(
                static_cast<const void *>(&*std::begin(copied))),
            static_cast<const std::byte *>(
                static_cast<const void *>(&*std::begin(copied))) +
                size(*std::begin(copied)) * std::size(copied),
            result);
}
}
// serialize in place contiguous lists of basic types, eg std::vector<uint8_t>
// or std::array<uint8_t, N>
template <std::ranges::sized_range R>
constexpr void serialize(std::weakly_incrementable auto result, const R &r)
  requires std::is_same_v<decltype(*result), std::byte &> &&
           basic_type<std::ranges::range_value_t<R>> &&
           std::ranges::contiguous_range<R>
{
  if (std::ranges::size(r) == 0) {
      return;
  }
  if constexpr (std::endian::native == std::endian::little) {
    _serialize_little_endian_basic_list(result, r);
  } else {
    _serialize_big_endian_basic_list(result, r);
  }
}
// serialize in place lists of variable size objects
template <std::ranges::sized_range R>
constexpr void serialize(std::weakly_incrementable auto result, const R &r)
  requires(std::is_same_v<decltype(*result), std::byte &> &&
           ssz_object_variable_size<std::ranges::range_value_t<R>>)
{
  auto data = result + std::ranges::size(r) * BYTES_PER_LENGTH_OFFSET;
  for (const auto &[idx, v] : std::views::enumerate(r)) {
    serialize(result + idx * BYTES_PER_LENGTH_OFFSET, static_cast<std::uint32_t>(std::distance(result, data)));
    serialize(data, v);
    std::advance(data, size(v));
  };
  result = data;
}
// serialize in place lists of fixed size objects (no offsets)
template <std::ranges::sized_range R>
constexpr void serialize(std::weakly_incrementable auto result, const R &r)
  requires(std::is_same_v<decltype(*result), std::byte &> &&
           ssz_object_fixed_size<std::ranges::range_value_t<R>> &&
           !basic_type<std::ranges::range_value_t<R>>)
{
  std::ranges::for_each(r, [&](const auto &v) {
      serialize(result, v);
      result += ssz::size(v);
  });
}
// serialize lists of fixed sized objects
constexpr auto serialize(const std::ranges::sized_range auto &r)
  requires ssz_object_fixed_size<std::ranges::range_value_t<decltype(r)>>
{
  std::vector<std::byte> ret(ssz::size(r));
  serialize(ret.begin(), r);
  return ret;
}
// serialize lists of variable sized objects
constexpr auto serialize(const std::ranges::sized_range auto &r)
  requires ssz_object_variable_size<std::ranges::range_value_t<decltype(r)>> &&
           std::ranges::contiguous_range<decltype(r)>
{
  auto total_size = std::ranges::fold_left(
      r, 0, [](std::uint32_t total, const auto &val) { return total + BYTES_PER_LENGTH_OFFSET + ssz::size(val); });
  std::vector<std::byte> ret(total_size);
  serialize(ret.begin(), r);
  return ret;
}

/*
 * bitvectors and bitlists are special.
 *
 * In general we will use std::bitset as a model for bitvector and ssz::list<bool, N> as a model for bitlists.
 * std::vector<bool> is used internally in ssz::list<bool, N> as a container and it should not be used as an SSZ
 * container explicitly. It serializes as a bitvector (since the size is known) and deserializes as a bitlist (since the
 * size is not known otherwise).
 */
namespace {
constexpr auto eight_bits_to_byte = [](const auto &chunk) {
  return std::ranges::fold_right(chunk, std::byte{}, [](auto i, std::byte b) {
    return (b << 1) | std::byte{i};
  });
};
constexpr void _serialize_bitvector(std::weakly_incrementable auto result,
                                    const std::ranges::sized_range auto &r) {
  std::ranges::transform(r | std::views::chunk(CHAR_BIT), result,
                         eight_bits_to_byte);
}
}
// serialize in place bitvectors modeled by std::vector<bool>
constexpr void serialize(std::weakly_incrementable auto result,
                         const std::vector<bool> &r)
  requires std::is_same_v<decltype(*result), std::byte &>
{
  _serialize_bitvector(result, r);
}
// serialize bitlist modeled by ssz::list<bool>
template <std::size_t N>
constexpr void serialize(std::weakly_incrementable auto result, const ssz::list<bool, N> &r)
    requires std::is_same_v<decltype(*result), std::byte &>
{
  _serialize_bitvector(result, r);
  *(result + r.size() / CHAR_BIT) |= std::byte{1} << (r.size() % CHAR_BIT);
}

// serialize bitlist modeled by std::vector<bool>
constexpr auto serialize(const std::vector<bool> &r) {
  std::vector<std::byte> ret{r.size() / CHAR_BIT + 1};
  serialize(ret.begin(), r);
  return ret;
}

// serialize bitlist modeled by ssz::list<bool>
template <std::size_t N>
constexpr auto serialize(const ssz::list<bool, N> &r) {
  std::vector<std::byte> ret{r.size() / CHAR_BIT + 1};
  serialize(ret.begin(), r);
  return ret;
}

// serialize in place a bitvector modeled by a bitset
template <std::weakly_incrementable R, std::size_t N>
constexpr void serialize(R result, const std::bitset<N> &r)
  requires std::is_same_v<decltype(*result), std::byte &>
{
  for (std::size_t i = 0; i < N; i++) {
    result[i / CHAR_BIT] |= std::byte{r[i]} << (i % CHAR_BIT);
  }
}

// serialize a bitvector modeled by a bitset
template <std::size_t N>
constexpr std::vector<std::byte> serialize(const std::bitset<N> &r) {
  std::vector<std::byte> ret{(r.size() + CHAR_BIT - 1) / CHAR_BIT};
  serialize(ret.begin(), r);
  return ret;
}
//
// Deserialization
// forward declaration

// helper to deserialize vectors of basic types
template <typename T>
constexpr void __deserialize(ssz_fixed_sized_vector auto &V, const serialized_range auto &bytes, auto length) {
  std::copy(static_cast<const T *>(static_cast<const void *>(&*std::begin(bytes))),
            static_cast<const T *>(static_cast<const void *>(&*std::begin(bytes))) + length, &*std::begin(V));
  if constexpr (std::endian::native == std::endian::big) std::ranges::for_each(V, std::byteswap);
}

template <ssz_object_fixed_size T>
constexpr auto vector_length(const serialized_range auto &bytes) {
  auto rsize = std::ranges::size(bytes);
  T vac{};
  auto vac_size = ssz::size(vac);  // TODO: GCC 13.1 dies if we use T{} here (&& rather than &)
  auto quot = rsize / vac_size;
  auto remainder = rsize % vac_size;
  if (remainder != 0) throw std::invalid_argument("not a multiple of the basic type size");
  return quot;
}

// deserialize vectors of basic types modeled on std::vector
template <basic_type T>
constexpr void deserialize(const serialized_range auto &bytes, std::vector<T> &ret) {
  auto quot = vector_length<T>(bytes);
  ret.resize(quot);
  __deserialize<T>(ret, bytes, quot);
}

// deserialize vectors of basic types modeled on std::array
template <basic_type T, std::size_t N>
constexpr void deserialize(const serialized_range auto &bytes, std::array<T, N> &ret) {
  auto quot = size(T{}) * N;
  if (std::ranges::size(bytes) < quot) throw std::invalid_argument("not enough bytes to deserialize vector");
  __deserialize<T>(ret, bytes, N);
}

// deserialized lists of basic types
template <basic_type T, std::size_t N>
constexpr void deserialize(const serialized_range auto &bytes, ssz::list<T, N> &ret) {
  deserialize(bytes, ret.data());
}

template <typename T>
    requires(!basic_type<T>)
constexpr void __deserialize_chunked(auto _ret, const auto &bytes) {
  T vac{};  // TODO GCC 13.1 dies if we use size(T{})
  auto chunk_size = ssz::size(vac);
  for (const auto &chunk : bytes | std::views::chunk(chunk_size)) {
    deserialize(chunk, *_ret);
    _ret++;
  }
}

// deserialize vectors of fixed sized types modeled as std::vector
template <ssz_object_fixed_size T>
    requires(!basic_type<T>)
constexpr auto deserialize(const serialized_range auto &bytes, std::vector<T> &ret) {
  auto quot = vector_length<T>(bytes);
  ret.clear();
  ret.resize(quot);
  __deserialize_chunked<T>(std::begin(ret), bytes);
}

// helper to deserialize vectors of fixed sized types modeled as std::array
template <ssz_object_fixed_size T, size_t N>
    requires(!basic_type<T>)
constexpr auto deserialize(const serialized_range auto &bytes, std::array<T, N> &ret) {
  __deserialize_chunked<T>(std::begin(ret), bytes);
}

// deserialize vectors of basic types modeled on c-style arrays, you're crazy to use this
template <ssz_object_fixed_size T, std::size_t N>
    requires(!basic_type<T>)
constexpr auto _deserialize(const serialized_range auto &bytes, T *&ret) {
  __deserialize_chunked<T>(std::begin(ret), bytes);
}

// helper to deserialize vectors of variable sized types modeled as std::vector
template <ssz_object_variable_size T>
auto deserialize(const serialized_range auto &bytes, std::vector<T> &ret) {
  ret.clear();
  if (std::ranges::size(bytes) == 0) {
    return;
  }

  // The first offset is enough to compute how many elements we need
  auto offset = *reinterpret_cast<const uint32_t *>(&*std::begin(bytes));
  ret.resize(offset / sizeof(uint32_t));
  auto idx = std::begin(bytes) + BYTES_PER_LENGTH_OFFSET;
  for (auto &elem : ret | std::views::take(ret.size() - 1)) {
    auto next_offset = *reinterpret_cast<const uint32_t *>(&*idx);
    deserialize(std::ranges::subrange(std::begin(bytes) + offset, std::begin(bytes) + next_offset), elem);
    offset = next_offset;
    idx += BYTES_PER_LENGTH_OFFSET;
  }
  deserialize(bytes | std::views::drop(offset), ret.back());
}

// deserialize non-basic type lists
template <ssz_object T, size_t N>
    requires(!basic_type<T>)
auto deserialize(const serialized_range auto &bytes, ssz::list<T, N> &ret) {
  deserialize(bytes, ret.data());
}

/**
 * \brief helper to deserialize bitlist modeled by std::vector<bool>
 *
 * Caution: in general, vectors model an actual vector according to SSZ, that is, fixed length objects. This is not the
 * case with std::vector<bool> as there is no way of knowing the end of it. So a std::vector<bool> should only be used
 * as a bit list, in which case it is better to use ssz::list<bool, N> and let this type take care of the limit.
 * ssz::list<bool, N> uses internally std::vector<bool> as a container
 */
auto deserialize(const serialized_range auto &bytes, std::vector<bool> &ret) {
  ret.clear();
  ret.reserve(std::ranges::size(bytes) * CHAR_BIT);
  std::ranges::for_each(
      bytes | std::views::take(std::ranges::size(bytes) - 1), [&](auto b) {
        for (auto j = 0; j < CHAR_BIT; ++j)
          ret.push_back(std::to_integer<std::uint8_t>(b >> j) & 1);
      });

  auto last = std::to_integer<std::uint8_t>(*std::prev(std::end(bytes)));
  for (auto i = 0; i < CHAR_BIT - std::countl_zero(last) - 1; ++i) ret.push_back((last >> i) & 1);
}

// helper to deserialize bitvectors modeled by bitset
template <std::size_t N>
auto deserialize(const serialized_range auto &bytes, std::bitset<N> &ret) {
  ret.reset();
  for (auto const [i, b] : std::views::enumerate(bytes))
    for (size_t j = 0; j < CHAR_BIT && CHAR_BIT * i + j < N; ++j)
      if (std::to_integer<std::uint8_t>(b >> j) & 1)
        ret.set(CHAR_BIT * i + j);
}

// helper to deserialize bitlists modeled by std::list<bool, N>
template <std::size_t N>
auto deserialize(const serialized_range auto &bytes, ssz::list<bool, N> &ret) {
  if (std::ranges::size(bytes) * CHAR_BIT > N) throw std::out_of_range("byte slice larger than list limit");
  deserialize(bytes, ret.data());
}
} // namespace ssz

#ifdef HAVE_YAML
// Yaml decoding of lists
template <ssz::ssz_object T, size_t N>
    requires(!std::is_same_v<std::remove_cvref_t<T>, std::byte>)
struct YAML::convert<ssz::list<T, N>> {
  static bool decode(const YAML::Node &node, ssz::list<T, N> &r) {
    std::vector<T> vec{};
    if (!YAML::convert<std::vector<T>>::decode(node, vec)) return false;
    r.reset(vec);
    return true;
  }
};
#endif
