// MIT License
//
// Copyright (c) 2023 ADCIRC Development Group
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_CIRCULARARRAY_H
#define GAHM_CIRCULARARRAY_H

#include <array>
#include <iostream>

namespace Gahm::Datatypes {

/**
 * @brief Circular array class
 * The circular array class is a wrapper around the std::array class that
 * allows for indexing beyond the size of the array.  This is accomplished
 * by using the modulo operator to wrap the index around to the beginning
 * of the array.
 * @tparam T Type of data to store in the array
 * @tparam array_size The size of the array
 */
template <typename T, int array_size>
class CircularArray {
 public:
  static_assert(array_size > 0, "Array size must be greater than 0");

  /**
   * @brief Default constructor
   */
  CircularArray() = default;

  /*
   * @brief Constructor from std::array
   */
  explicit CircularArray(std::array<T, array_size> arr)
      : m_data(std::move(arr)) {}

  /* @brief Indexing operator */
  T &operator[](int index) noexcept { return m_data[mod_floor(index)]; }

  /* @brief Indexing operator */
  const T &operator[](int index) const noexcept {
    return m_data[mod_floor(index)];
  }

  /* @brief Copy assignment operator */
  Gahm::Datatypes::CircularArray<T, array_size> &operator=(
      const Gahm::Datatypes::CircularArray<T, array_size> &arr) noexcept {
    m_data = arr.m_data;
    return *this;
  }

  /* @brief Copy assignment operator */
  Gahm::Datatypes::CircularArray<T, array_size> &operator=(
      const std::array<T, array_size> &arr) noexcept {
    m_data = arr;
    return *this;
  }

  /* @brief Returns an iterator to the mod_floor of the index */
  auto iterator_at(int position) const noexcept {
    auto p = mod_floor(position);
    return m_data.begin() + p;
  }

  /* @brief Returns the value at the mod_floor of the index */
  auto at(int index) const noexcept { return m_data[mod_floor(index)]; }

  /* @brief Sets the value at the mod_floor of the index */
  void set(int index, T value) noexcept { m_data[mod_floor(index)] = value; }

  /* @brief Sets the values of the array using a std::array */
  void set(const std::array<T, array_size> &array) { m_data = array; }

  /* @brief Returns the front of the array */
  auto front() noexcept { return m_data.front(); }

  /* @brief Returns the back of the array */
  auto back() noexcept { return m_data.back(); }

  /* @brief Returns a pointer to the data */
  auto data() noexcept { return &m_data; }

  /* @brief Returns a const pointer to the data */
  auto data() const noexcept { return &m_data; }

  /* @brief returns true if the array is empty */
  auto empty() const noexcept { return m_data.empty(); }

  /* @brief returns the max size of the array */
  auto max_size() const noexcept { return m_data.max_size(); }

  /* @brief returns the size of the array */
  constexpr auto size() const noexcept { return array_size; }

  /* @brief Fills the array with a value */
  void fill(const T value) { m_data.fill(value); }

  /* @brief Returns an iterator to the beginning of the array */
  auto begin() noexcept { return m_data.begin(); }

  /* @brief Returns an iterator to the end of the array */
  auto end() noexcept { return m_data.end(); }

  /* @brief Returns a reverse iterator to the beginning of the array */
  auto rbegin() noexcept { return m_data.rbegin(); }

  /* @brief Returns a reverse iterator to the end of the array */
  auto rend() noexcept { return m_data.rend(); }

  /* @brief Returns an iterator to the beginning of the array */
  auto begin() const noexcept { return m_data.cbegin(); }

  /* @brief Returns an iterator to the end of the array */
  auto end() const noexcept { return m_data.cend(); }

  /* @brief Returns a reverse iterator to the beginning of the array */
  auto rbegin() const noexcept { return m_data.crbegin(); }

  /* @brief Returns a reverse iterator to the end of the array */
  auto rend() const noexcept { return m_data.crend(); }

  /* @brief Copy constructor
   * @param[in] arr The array to copy
   */
  CircularArray(const CircularArray<T, array_size> &arr) noexcept = default;

  /* @brief Equality operator */
  bool operator==(const CircularArray<T, array_size> &b) const {
    return m_data == b.m_data;
  }

  /* @brief Not equal operator */
  bool operator!=(const CircularArray<T, array_size> &b) const {
    return m_data != b.m_data;
  }

  /* @brief Less than operator */
  bool operator<(const CircularArray<T, array_size> &b) const {
    return m_data < b.m_data;
  }

  /* @brief Greater than operator */
  bool operator>(const CircularArray<T, array_size> &b) const {
    return m_data > b.m_data;
  }

  /* @brief Less than or equal to operator */
  bool operator<=(const CircularArray<T, array_size> &b) const {
    return m_data <= b.m_data;
  }

  /* @brief Greater than or equal to operator */
  bool operator>=(const CircularArray<T, array_size> &b) const {
    return m_data >= b.m_data;
  }

 private:
  std::array<T, array_size> m_data;

  /* @brief Returns the mod_floor of the index */
  static size_t mod_floor(int position) noexcept {
    if (position >= 0 && position < array_size) {
      return position;
    } else {
      if (position < 0) position += array_size;
      return position % array_size;
    }
  }
};
}  // namespace Gahm::Datatypes

template <typename T, size_t array_size>
std::ostream &operator<<(
    std::ostream &os,
    const Gahm::Datatypes::CircularArray<T, array_size> &array) {
  for (auto b = array.cbegin(); b != array.cend(); ++b) {
    os << *(b);
    if (b != array.cend() - 1) {
      os << ", ";
    }
  }
  return os;
}

#endif  // GAHM_CIRCULARARRAY_H
