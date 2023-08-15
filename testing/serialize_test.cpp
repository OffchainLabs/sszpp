/*  serialize_test.cpp
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
#include <iostream>
#include <limits>

#include "acutest.h"
#include "bytelists.hpp"
#include "concepts.hpp"
#include "ssz++.hpp"

void test_basic_types() {
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint8_t{3})[0]) == std::uint8_t{3});
  TEST_CHECK(ssz::serialize(std::uint8_t{3}).size() == 1);
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint16_t{3})[0]) == std::uint8_t{3});
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint16_t{3})[1]) == std::uint8_t{0});
  TEST_CHECK(ssz::serialize(std::uint16_t{3}).size() == 2);

  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint32_t{3})[0]) == std::uint8_t{3});
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint32_t{3})[1]) == std::uint8_t{0});
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint32_t{3})[2]) == std::uint8_t{0});
  TEST_CHECK(std::to_integer<std::uint8_t>(
                 ssz::serialize(std::uint32_t{3})[3]) == std::uint8_t{0});
  TEST_CHECK(ssz::serialize(std::uint32_t{3}).size() == 4);

  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[0]) == std::uint8_t{0x04});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[1]) == std::uint8_t{0x03});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[2]) == std::uint8_t{0x02});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[3]) == std::uint8_t{0x01});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[4]) == std::uint8_t{0x99});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[5]) == std::uint8_t{0xef});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[6]) == std::uint8_t{0xcd});
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(std::uint64_t{
                 0xabcdef9901020304ull})[7]) == std::uint8_t{0xab});
  TEST_CHECK(ssz::serialize(std::uint64_t{3}).size() == 8);

  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(true)[0]) == 1);
  TEST_CHECK(std::to_integer<std::uint8_t>(ssz::serialize(false)[0]) == 0);
  TEST_CHECK(ssz::serialize(true).size() == 1);
}
void test_serialize_in_place() {
  std::vector<std::byte> v{8};
  ssz::serialize(v.begin(), std::uint8_t{0x03});
  TEST_CHECK(v[0] == std::byte{0x03});

  ssz::serialize(++v.begin(), std::uint16_t{0x0102});
  TEST_CHECK(v[0] == std::byte{0x03});
  TEST_CHECK(v[1] == std::byte{0x02});
  TEST_CHECK(v[2] == std::byte{0x01});

  ssz::serialize(v.begin(), std::uint64_t{0x0102030405060708});
  TEST_CHECK(v[0] == std::byte{0x08});
  TEST_CHECK(v[1] == std::byte{0x07});
  TEST_CHECK(v[2] == std::byte{0x06});
  TEST_CHECK(v[3] == std::byte{0x05});
  TEST_CHECK(v[4] == std::byte{0x04});
  TEST_CHECK(v[5] == std::byte{0x03});
  TEST_CHECK(v[6] == std::byte{0x02});
  TEST_CHECK(v[7] == std::byte{0x01});

  ssz::serialize(v.begin(), true);
  TEST_CHECK(v[0] == std::byte{0x01});

  ssz::serialize(++v.begin(), false);
  TEST_CHECK(v[1] == std::byte{0x00});

  // check we can use a view
  auto myview = v | std::views::take(3);
  ssz::serialize(myview.begin(), std::uint8_t{0xff});
  TEST_CHECK(v[0] == std::byte{0xff});

  // check we can use an array
  std::array<std::byte, 8> myarray{};
  ssz::serialize(myarray.begin(), std::uint16_t{0xffee});
  TEST_CHECK(myarray[0] == std::byte{0xee});
}
void test_deserialize_basic_types() {
  std::vector<std::byte> bytes{
      std::byte{0x01}, std::byte{0x02}, std::byte{0x03}, std::byte{0x04},
      std::byte{0x05}, std::byte{0x06}, std::byte{0x07}, std::byte{0x08}};

  TEST_CHECK(ssz::deserialize<std::uint8_t>(bytes) == 0x01);
  TEST_CHECK(ssz::deserialize<std::uint16_t>(bytes) == 0x0201);
  TEST_CHECK(ssz::deserialize<std::uint32_t>(bytes) == 0x04030201);
  TEST_CHECK(ssz::deserialize<std::uint64_t>(bytes) == 0x0807060504030201);

  TEST_EXCEPTION(ssz::deserialize<std::uint64_t>(bytes | std::views::take(7)),
                 std::invalid_argument);

  TEST_CHECK(ssz::deserialize<bool>(bytes));
  bytes[0] = std::byte{0x00};
  TEST_CHECK(!ssz::deserialize<bool>(bytes));
  bytes[0] = std::byte{0x02};
  TEST_EXCEPTION(ssz::deserialize<bool>(bytes), std::invalid_argument);

  ssz::list<bool, 10> mylist{std::vector<bool>(3)};
}
void test_serialize_fixed_sized_lists() {
  std::vector<std::byte> result{16};
  std::uint32_t values[3] = {0x0a0b0c0d, 0x01020304, 0xaabbccdd};
  ssz::serialize(result.begin(), values);
  std::vector<std::byte> expected{
      std::byte{0x0d}, std::byte{0x0c}, std::byte{0x0b}, std::byte{0x0a},
      std::byte{0x04}, std::byte{0x03}, std::byte{0x02}, std::byte{0x01},
      std::byte{0xdd}, std::byte{0xcc}, std::byte{0xbb}, std::byte{0xaa},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  TEST_CHECK(std::ranges::equal(result | std::views::take(12),
                                expected | std::views::take(12)));

  std::array<std::uint16_t, 4> values_16{0xbbaa, 0xddcc, 0x2211, 0x4433};
  ssz::serialize(++result.begin(), values_16);
  std::vector<std::byte> expected_16{
      std::byte{0xaa}, std::byte{0xbb}, std::byte{0xcc}, std::byte{0xdd},
      std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}};
  TEST_CHECK(std::ranges::equal(
      result | std::views::drop(1) | std::views::take(8), expected_16));

  std::vector<std::uint32_t> values_32{0xffeeddcc, 0x44332211};
  std::vector<std::byte> expected_32{
      std::byte{0xcc}, std::byte{0xdd}, std::byte{0xee}, std::byte{0xff},
      std::byte{0x11}, std::byte{0x22}, std::byte{0x33}, std::byte{0x44}};
  ssz::serialize(result.begin(), values_32);
  TEST_CHECK(std::ranges::equal(result | std::views::take(8), expected_32));
  auto result_32 = ssz::serialize(values_32);
  TEST_CHECK(std::ranges::equal(result_32, expected_32));

  std::vector<bool> values_bool{true, true, false, false, true};
  auto result_bool = ssz::serialize(values_bool);
  std::vector<std::byte> expected_bool{std::byte{0b10011}};
  std::vector<std::byte> expected_hexa_bool{std::byte{0x13}};
  TEST_CHECK(std::ranges::equal(result_bool, expected_bool));
  TEST_CHECK(std::ranges::equal(result_bool, expected_hexa_bool));

  ssz::list<bool, 10> list_bool{values_bool};
  auto result_list_bool = ssz::serialize(list_bool);
  std::vector<std::byte> expected_bool_list{std::byte{0b110011}};
  std::vector<std::byte> expected_hexa_bool_list{std::byte{0x33}};
  TEST_CHECK(std::ranges::equal(result_list_bool, expected_bool_list));
  TEST_CHECK(std::ranges::equal(result_list_bool, expected_hexa_bool_list));

  std::bitset<5> values_bool_bitset{"11001"};
  auto result_bool_bitset = ssz::serialize(values_bool_bitset);
  std::vector<std::byte> expected_bool_bitset{std::byte{0b11001}};
  TEST_CHECK(std::ranges::equal(result_bool_bitset, expected_bool_bitset));

  TEST_CHECK(ssz::size(values_bool_bitset) == 1);
  TEST_CHECK(ssz::size(values_16) == 8);
  TEST_CHECK(ssz::size(values_bool) == 1);
  std::vector<std::uint64_t> values_64{0, 0, 0, 0};

  ssz::list<std::uint32_t, std::numeric_limits<std::uint32_t>::max()> list_32{values_32};
  static_assert(std::ranges::range<decltype(values_32)>);
  static_assert(std::ranges::input_range<decltype(list_32)>);
  static_assert(ssz::ssz_object_fixed_size<
                std::ranges::range_value_t<decltype(list_32)>>);
  TEST_CHECK(ssz::size(list_32) == 8);
  ssz::serialize(result.begin(), list_32);
  TEST_CHECK(std::ranges::equal(result | std::views::take(8), expected_32));
}
void test_bitlists_sizes() {
  std::bitset<24> bitset{"111000111100111000111100"};
  std::vector<bool> vector(24);
  ssz::list<bool, 200> list{vector};
  TEST_CHECK(ssz::size(bitset) == 3);
  TEST_CHECK(ssz::size(vector) == 3);
  TEST_CHECK(ssz::size(list) == 4);
}
void test_list_of_lists() {
  std::vector<ssz::list<std::uint64_t, 20>> vec{};
  vec.push_back({{0xabcdef9901020304ull, 0x01ull, 0xaaaaull}});
  vec.push_back({{0x901020304ac31ull, 0x223301ull}});
  vec.push_back({{0xffffef9241320304ull, 0xaabbbbbbbbull, 0xaaaaull, 0x0ull, 0x1ull}});
  auto result = ssz::serialize(vec);
  TEST_CHECK(result.size() == 92);
  TEST_MSG("Obtained %ld", result.size());

  ssz::list<ssz::list<std::uint64_t, 20>, 30> double_list{vec};
  auto result2 = serialize(double_list);
  TEST_CHECK(result2.size() == 92);
  TEST_CHECK(std::ranges::equal(result, result2));
}
void test_deserialize_basic_list() {
  std::vector<std::byte> bytes{
      std::byte{0x0d}, std::byte{0x0c}, std::byte{0x0b}, std::byte{0x0a},
      std::byte{0x04}, std::byte{0x03}, std::byte{0x02}, std::byte{0x01},
      std::byte{0xdd}, std::byte{0xcc}, std::byte{0xbb}, std::byte{0xaa}};
  std::vector<std::uint32_t> expected{0x0a0b0c0d, 0x01020304, 0xaabbccdd};
  auto result = ssz::deserialize<std::vector<std::uint32_t>>(bytes);
  TEST_CHECK(std::ranges::equal(result, expected));

  auto result_array = ssz::deserialize<std::array<std::uint32_t, 3>>(bytes);
  TEST_CHECK(std::ranges::equal(result, result_array));

  auto result_list = ssz::deserialize<ssz::list<std::uint32_t, 200>>(bytes);
  TEST_CHECK(std::ranges::equal(result, result_list));
  TEST_CHECK(result_list.limit() == 200);

  std::vector<bool> expected_bool{true, true, false, false, true};
  std::vector<std::byte> bytes_bool{std::byte{0b00110011}};
  auto result_bool = ssz::deserialize<std::vector<bool>>(bytes_bool);
  TEST_CHECK(std::ranges::equal(result_bool, expected_bool));

  std::bitset<14> expected_bool_bitset{"10011000111010"};
  std::vector<std::byte> serialized_bitset{std::byte{0b00111010},
                                           std::byte{0b00100110}};
  auto result_bool_bitset =
      ssz::deserialize<std::bitset<14>>(serialized_bitset);
  TEST_CHECK(result_bool_bitset == expected_bool_bitset);
}

void test_list_of_vectors() {
  std::vector<std::array<std::uint64_t, 3>> vec{};
  vec.push_back({0xabcdef9901020304ull, 0x01ull, 0xaaaaull});
  vec.push_back({0x901020304ac31ull, 0x223301ull, 0x0ull});
  vec.push_back({0xffffef9241320304ull, 0xaabbbbbbbbull, 0xaaaaull});
  auto result = ssz::serialize(vec);
  std::vector<std::byte> expected{
      std::byte{0x04}, std::byte{0x03}, std::byte{0x02}, std::byte{0x01}, std::byte{0x99}, std::byte{0xef},
      std::byte{0xcd}, std::byte{0xab}, std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0xaa}, std::byte{0xaa},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x31}, std::byte{0xac}, std::byte{0x04}, std::byte{0x03}, std::byte{0x02}, std::byte{0x01},
      std::byte{0x09}, std::byte{0x00}, std::byte{0x01}, std::byte{0x33}, std::byte{0x22}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x04}, std::byte{0x03}, std::byte{0x32}, std::byte{0x41}, std::byte{0x92}, std::byte{0xef},
      std::byte{0xff}, std::byte{0xff}, std::byte{0xbb}, std::byte{0xbb}, std::byte{0xbb}, std::byte{0xbb},
      std::byte{0xaa}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0xaa}, std::byte{0xaa},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}};
  TEST_CHECK(result.size() == 72);
  TEST_CHECK(result == expected);
  TEST_MSG("Obtained %ld", result.size());

  ssz::list<std::array<std::uint64_t, 3>, 100> double_list{vec};
  auto result2 = serialize(double_list);
  TEST_CHECK(result2.size() == 72);
  TEST_CHECK(std::ranges::equal(result, result2));

  auto round = ssz::deserialize<std::vector<std::array<std::uint64_t, 3>>>(result);
  TEST_CHECK(round == vec);
}

struct my_container_t : ssz::ssz_container {
  std::uint32_t a{};
  std::uint16_t b{};
  bool c{};
  SSZ_CONT(a, b, c);
};

struct my_variable_container_t : ssz::ssz_variable_size_container {
  std::uint32_t a{3};
  std::uint16_t b{7};
  ssz::list<std::uint64_t, 100> vec{std::vector<std::uint64_t>(16)};
  bool c{true};
  SSZ_CONT(a, b, vec, c);
};

static_assert(ssz::ssz_object_variable_size<my_variable_container_t>);

void test_serialize_containers() {
  std::vector<std::byte> result{32};
  std::uint32_t a{3};
  std::uint16_t b{7};
  bool c{true};
  ssz::list<std::uint64_t, 100> vec{std::vector<std::uint64_t>(16)};
  ssz::serialize(result.begin(), a, b, c);
  TEST_CHECK(ssz::size(b) == 2);
  TEST_CHECK(ssz::size_or_placeholder(b) == 2);
  TEST_MSG("Obtained %d:", ssz::size_or_placeholder(b));
  static_assert(ssz::ssz_object_fixed_size<decltype(b)>);
  TEST_CHECK(ssz::compute_fixed_length(a, b, c) == 7);
  TEST_CHECK(ssz::compute_fixed_length(a, b, vec, c) == 11);
  TEST_CHECK(ssz::compute_fixed_length(a, b, vec, c, result) == 11 + 32);
  ssz::list<bool, 200> bool_list{std::vector<bool>{true, false, true}};
  TEST_CHECK(ssz::compute_fixed_length(a, b, vec, c, result, bool_list) == 11 + 32 + 4);

  my_container_t my_container;
  my_container.a = 3;
  my_container.b = 7;
  my_container.c = true;

  auto cont_result = ssz::serialize(my_container);
  TEST_CHECK(cont_result.size() == 7);
  TEST_CHECK(std::ranges::equal(result | std::views::take(7), cont_result));
  std::vector<std::byte> expected{std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
                                  std::byte{0x07}, std::byte{0x00}, std::byte{0x01}};
  std::vector<std::uint8_t> transformed;
  std::ranges::transform(cont_result, std::back_inserter(transformed), std::to_integer<std::uint8_t>);
  std::cout << std::hex;
  auto cont_result_str =
      std::ranges::fold_left(transformed, "", [](std::string s, auto b) { return s + std::to_string(b) + ", "; });
  TEST_CHECK(cont_result == expected);
  TEST_MSG(cont_result_str.c_str());

  my_variable_container_t variable_container{};
  auto& my_vec = variable_container.vec;
  static_assert(ssz::ssz_object_variable_size<decltype(variable_container.vec)>);
  static_assert(ssz::ssz_object_variable_size<decltype(my_vec)>);
  auto cont_result_variable = ssz::serialize(variable_container);
  std::vector<std::uint8_t> transformed_variable;
  std::ranges::transform(cont_result_variable, std::back_inserter(transformed_variable), std::to_integer<std::uint8_t>);
  std::vector<std::byte> expected_variable{
      std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x07}, std::byte{0x00},
      std::byte{0x0b}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}};
  auto cont_result_variable_str = std::ranges::fold_left(
      transformed_variable, "", [](std::string s, auto b) { return s + std::to_string(b) + ", "; });
  TEST_CHECK(cont_result_variable.size() == 139);
  TEST_CHECK(expected_variable.size() == 139);
  TEST_CHECK(cont_result_variable == expected_variable);
  TEST_MSG(cont_result_variable_str.c_str());
}
void test_deserialize_containers() {
  std::vector<std::byte> expected{std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
                                  std::byte{0x07}, std::byte{0x00}, std::byte{0x01}};
  auto deserialized_cont = ssz::deserialize<my_container_t>(expected);
  TEST_CHECK(deserialized_cont.a == 3);
  TEST_CHECK(deserialized_cont.b == 7);
  TEST_CHECK(deserialized_cont.c);

  my_variable_container_t variable_container{};
  auto& my_vec = variable_container.vec;
  my_vec[3] = 0x05ull;
  variable_container.a = 5;
  std::vector<std::byte> expected_variable{
      std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x07}, std::byte{0x00},
      std::byte{0x0b}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x01}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x05},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
      std::byte{0x00}};

  auto deserialized_variable = ssz::deserialize<my_variable_container_t>(expected_variable);
  TEST_CHECK(std::ranges::equal(my_vec, deserialized_variable.vec));
  TEST_CHECK(deserialized_variable.a == 5);
}
void test_hextring_to_bytes() {
  auto hexstring = "0xff21";
  std::array<std::byte, 2> byte_array{};
  std::vector<std::byte> byte_vector(2);
  ssz::bytelist_from_str(hexstring, byte_array);
  ssz::bytelist_from_str(hexstring, byte_vector);
  TEST_CHECK(byte_array[0] == std::byte{0xff});
  TEST_CHECK(byte_array[1] == std::byte{0x21});
  TEST_CHECK(byte_vector[0] == std::byte{0xff});
  TEST_CHECK(byte_vector[1] == std::byte{0x21});
}
TEST_LIST{{"basic_types", test_basic_types},
          {"serialize_in_place", test_serialize_in_place},
          {"deserialize_basic_types", test_deserialize_basic_types},
          {"serialize_fixed_sized_lists", test_serialize_fixed_sized_lists},
          {"bitlist_sizes", test_bitlists_sizes},
          {"list_of_lists", test_list_of_lists},
          {"deserialize_basic_list", test_deserialize_basic_list},
          {"list_of_vectors", test_list_of_vectors},
          {"serialize_containers", test_serialize_containers},
          {"deserialize_containers", test_deserialize_containers},
          {"hexstring_to_bytes", test_hextring_to_bytes},
          {NULL, NULL}};
