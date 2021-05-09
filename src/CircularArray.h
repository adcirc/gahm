#ifndef CIRCULARARRAY_H
#define CIRCULARARRAY_H

#include <array>

template <typename T, size_t array_size>
class CircularArray {
 public:
  CircularArray() = default;

  explicit CircularArray(std::array<T, array_size> arr)
      : m_data(std::move(arr)) {}

  constexpr size_t size() noexcept { return array_size; }

  auto &data() const noexcept { return m_data; }

  const auto &cdata() const noexcept { return m_data; }

  T &operator[](long index) noexcept { return m_data[mod_floor(index)]; }

  const T &operator[](long index) const noexcept {
    return m_data[mod_floor(index)];
  }

  CircularArray<T, array_size> &operator=(
      const CircularArray<T, array_size> &arr) noexcept {
    m_data = arr.data();
    return *this;
  }

  auto at(long index) const noexcept { return m_data[mod_floor(index)]; }
  void set(long index, T value) noexcept { m_data[mod_floor(index)] = value; }

  auto begin() noexcept { return m_data.begin(); }
  auto end() noexcept { return m_data.end(); }
  auto cbegin() const noexcept { return m_data.cbegin(); }
  auto cend() const noexcept { return m_data.cend(); }

 private:
  std::array<T, array_size> m_data;

  static constexpr size_t mod_floor(long position) noexcept {
    return ((position % array_size) + array_size) % array_size;
  }
};

#endif  // CIRCULARARRAY_H
