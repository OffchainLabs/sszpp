/*  concepts_test.cpp
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

#include <cstdint>
#include <string>
#include <type_traits>

#include "acutest.h"
#include "basic_types.hpp"
#include "ssz++.hpp"

bool test_basic(ssz::basic_type auto i) { return true; }
bool test_basic(auto x) { return false; }

void test_basic_type() {
  TEST_CHECK(test_basic(3) == false);
  TEST_CHECK(test_basic(false) == true);
  TEST_CHECK(test_basic(true) == true);
  TEST_CHECK(test_basic(std::uint8_t{3}) == true);
  TEST_CHECK(test_basic(std::uint16_t{3}) == true);
  TEST_CHECK(test_basic(std::uint32_t{3}) == true);
  TEST_CHECK(test_basic(std::uint64_t{3}) == true);
  TEST_CHECK(test_basic(std::byte{0x03}) == true);
  TEST_CHECK(test_basic("32") == false);
  TEST_CHECK(test_basic(std::int8_t{3}) == false);
  static_assert(!ssz::basic_type<int>);
  static_assert(ssz::basic_type<std::uint8_t>);
  static_assert(!ssz::basic_type<std::string>);
  static_assert(ssz::basic_type<bool>);
  static_assert(ssz::ssz_object_fixed_size<std::uint16_t>);
  static_assert(ssz::ssz_object_fixed_size<std::uint32_t>);
  static_assert(ssz::ssz_object_fixed_size<bool>);

  std::uint32_t a{3};
  const auto b = a;
  auto& c = a;
  static_assert(ssz::ssz_object_fixed_size<decltype(a)>);
  static_assert(ssz::ssz_object_fixed_size<decltype(b)>);
  static_assert(ssz::ssz_object_fixed_size<decltype(c)>);
}

void test_array() {
  static_assert(ssz::ssz_object<std::uint8_t>);
  static_assert(ssz::ssz_object<std::array<std::uint8_t, 4>>);
  static_assert(ssz::ssz_object<std::vector<std::uint8_t>>);
  static_assert(ssz::ssz_object<std::vector<std::vector<bool>>>);
  static_assert(ssz::ssz_object<uint8_t[3]>);
  static_assert(!ssz::ssz_object<uint8_t[]>);
  static_assert(ssz::ssz_object<std::array<std::vector<std::uint8_t>, 4>>);
  static_assert(ssz::ssz_object<std::bitset<4>>);
  static_assert(!ssz::ssz_fixed_sized_vector<std::vector<bool>>);
  static_assert(ssz::ssz_fixed_sized_vector<std::vector<std::uint8_t>>);
  static_assert(std::is_array_v<std::uint8_t[10]>);
  static_assert(ssz::ssz_object<std::remove_pointer_t<std::decay_t<std::uint8_t[10]>>>);
  static_assert(!std::is_same_v<std::decay_t<std::uint8_t[10]>, bool*>);
  static_assert(ssz::ssz_vector<std::uint8_t[10]>);
  static_assert(ssz::ssz_fixed_sized_vector<std::uint8_t[10]>);
  static_assert(ssz::ssz_fixed_sized_vector<std::array<std::uint8_t, 10>>);
  static_assert(!ssz::ssz_fixed_sized_vector<std::vector<int>>);
}

void test_variable_size() {
  static_assert(ssz::list<std::uint8_t, 20>::variable_size::value);
  static_assert(ssz::ssz_object_variable_size<ssz::list<std::uint8_t, 0>>);
  static_assert(!ssz::ssz_object_fixed_size<ssz::list<std::uint8_t, 30>>);
}

TEST_LIST{{"basic_type", test_basic_type}, {"array", test_array}, {NULL, NULL}};
