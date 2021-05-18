// MIT License
//
// Copyright (c) 2020 ADCIRC Development Group
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
#ifndef CIRCULARARRAY_H
#define CIRCULARARRAY_H

#include <array>

template <typename T, size_t array_size>
class CircularArray {
 public:
  CircularArray() = default;

  explicit CircularArray(std::array<T, array_size> arr)
      : m_data(std::move(arr)) {}

  T &operator[](long index) noexcept { return m_data[mod_floor(index)]; }

  const T &operator[](long index) const noexcept {
    return m_data[mod_floor(index)];
  }

  CircularArray<T, array_size> &operator=(
      const CircularArray<T, array_size> &arr) noexcept {
    m_data = arr.m_data;
    return *this;
  }

  CircularArray<T, array_size> &operator=(
      const std::array<T, array_size> &arr) noexcept {
    m_data = arr;
    return *this;
  }

  auto at(long index) const noexcept { return m_data[mod_floor(index)]; }
  void set(long index, T value) noexcept { m_data[mod_floor(index)] = value; }

  auto front() noexcept { return m_data.front(); }
  auto back() noexcept { return m_data.back(); }

  auto data() noexcept { return &m_data; }
  auto data() const noexcept { return &m_data; }

  auto empty() const noexcept { return m_data.empty(); }
  auto max_size() const noexcept { return m_data.max_size(); }
  constexpr auto size() const noexcept { return array_size; }

  void fill(const T value) { m_data.fill(value); }

  auto begin() noexcept { return m_data.begin(); }
  auto end() noexcept { return m_data.end(); }

  auto rbegin() noexcept { return m_data.rbegin(); }
  auto rend() noexcept { return m_data.rend(); }

  auto cbegin() const noexcept { return m_data.cbegin(); }
  auto cend() const noexcept { return m_data.cend(); }

  auto crbegin() const noexcept { return m_data.crbegin(); }
  auto crend() const noexcept { return m_data.crend(); }

  bool operator==(const CircularArray<T, array_size> &b) const {
    return m_data == b.m_data;
  }

  bool operator!=(const CircularArray<T, array_size> &b) const {
    return m_data != b.m_data;
  }

  bool operator<(const CircularArray<T, array_size> &b) const {
    return m_data < b.m_data;
  }

  bool operator>(const CircularArray<T, array_size> &b) const {
    return m_data > b.m_data;
  }

  bool operator<=(const CircularArray<T, array_size> &b) const {
    return m_data <= b.m_data;
  }

  bool operator>=(const CircularArray<T, array_size> &b) const {
    return m_data >= b.m_data;
  }

 private:
  std::array<T, array_size> m_data;

  static constexpr size_t mod_floor(long position) noexcept {
    return ((position % array_size) + array_size) % array_size;
  }
};

#endif  // CIRCULARARRAY_H
