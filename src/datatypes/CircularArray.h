// GNU General Public License v3.0
//
// This file is part of the GAHM model (https://github.com/adcirc/gahm).
// Copyright (c) 2023 ADCIRC Development Group.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Zach Cobell
// Contact: zcobell@thewaterinstitute.org
//
#ifndef GAHM_CIRCULARARRAY_H
#define GAHM_CIRCULARARRAY_H

#include <array>
#include <iostream>
#include <vector>

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
template <typename T, unsigned array_size>
class CircularArray {
 public:
  static_assert(array_size > 0, "Array size must be greater than 0");

  /**
   * @brief Default constructor
   */
  constexpr CircularArray() = default;

  /*
   * @brief Constructor from std::array
   */
  constexpr explicit CircularArray(std::array<T, array_size> arr)
      : m_data(std::move(arr)) {}

  /* @brief Indexing operator */
  constexpr T &operator[](int index) noexcept {
    return m_data[mod_floor(index)];
  }

  /* @brief Indexing operator */
  constexpr const T &operator[](int index) const noexcept {
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
  template <typename Tp>
  constexpr auto iterator_at(Tp position) const noexcept {
    auto p = mod_floor(position);
    return m_data.begin() + p;
  }

  /* @brief Returns the value at the index without the mod_floor */
  template <typename Tp>
  constexpr auto at(Tp index) const noexcept {
    return m_data[index];
  }

  /* @brief Sets the value at the mod_floor of the index */
  template <typename Tp>
  constexpr void set(Tp index, T value) noexcept {
    m_data[mod_floor<Tp>(index)] = value;
  }

  /* @brief Sets the values of the array using a std::array */
  constexpr void set(const std::array<T, array_size> &array) { m_data = array; }

  /* @brief Returns the front of the array */
  constexpr auto front() noexcept { return m_data.front(); }

  /* @brief Returns the back of the array */
  constexpr auto back() noexcept { return m_data.back(); }

  /* @brief Returns a pointer to the data */
  constexpr auto data() noexcept { return &m_data; }

  /* @brief Returns a const pointer to the data */
  constexpr auto data() const noexcept { return &m_data; }

  /* @brief Return the data as a vector */
  std::vector<T> asVector() const noexcept {
    return std::vector<T>(m_data.begin(), m_data.end());
  }

  /* @brief returns true if the array is empty */
  constexpr auto empty() const noexcept { return m_data.empty(); }

  /* @brief returns the max size of the array */
  constexpr auto max_size() const noexcept { return m_data.max_size(); }

  /* @brief returns the size of the array */
  constexpr auto size() const noexcept { return array_size; }

  /* @brief Fills the array with a value */
  constexpr void fill(const T value) { m_data.fill(value); }

  /* @brief Returns an iterator to the beginning of the array */
  constexpr auto begin() noexcept { return m_data.begin(); }

  /* @brief Returns an iterator to the end of the array */
  constexpr auto end() noexcept { return m_data.end(); }

  /* @brief Returns a reverse iterator to the beginning of the array */
  constexpr auto rbegin() noexcept { return m_data.rbegin(); }

  /* @brief Returns a reverse iterator to the end of the array */
  constexpr auto rend() noexcept { return m_data.rend(); }

  /* @brief Returns an iterator to the beginning of the array */
  constexpr auto begin() const noexcept { return m_data.cbegin(); }

  /* @brief Returns an iterator to the end of the array */
  constexpr auto end() const noexcept { return m_data.cend(); }

  /* @brief Returns a reverse iterator to the beginning of the array */
  constexpr auto rbegin() const noexcept { return m_data.crbegin(); }

  /* @brief Returns a reverse iterator to the end of the array */
  constexpr auto rend() const noexcept { return m_data.crend(); }

  /* @brief Copy constructor
   * @param[in] arr The array to copy
   */
  constexpr CircularArray(const CircularArray<T, array_size> &arr) = default;

  /* @brief Equality operator */
  constexpr bool operator==(const CircularArray<T, array_size> &b) const {
    return m_data == b.m_data;
  }

  /* @brief Not equal operator */
  constexpr bool operator!=(const CircularArray<T, array_size> &b) const {
    return m_data != b.m_data;
  }

  /* @brief Less than operator */
  constexpr bool operator<(const CircularArray<T, array_size> &b) const {
    return m_data < b.m_data;
  }

  /* @brief Greater than operator */
  constexpr bool operator>(const CircularArray<T, array_size> &b) const {
    return m_data > b.m_data;
  }

  /* @brief Less than or equal to operator */
  constexpr bool operator<=(const CircularArray<T, array_size> &b) const {
    return m_data <= b.m_data;
  }

  /* @brief Greater than or equal to operator */
  constexpr bool operator>=(const CircularArray<T, array_size> &b) const {
    return m_data >= b.m_data;
  }

 private:
  std::array<T, array_size> m_data;

  /* @brief Returns the mod_floor of the index */
  template <typename Tp>
  static constexpr unsigned mod_floor(Tp position) noexcept {
    return static_cast<unsigned>(
        (static_cast<unsigned>(position) + array_size) % array_size);
  }
};
}  // namespace Gahm::Datatypes

template <typename T, unsigned array_size>
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
