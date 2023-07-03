/**
 * Copyright (C) 2023 Sebastiano Smaniotto - All rights reserved
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more impls.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <cmath>
#include <iostream>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

#include "dataframe_impl.hpp"
#include "iterator.hpp"

namespace detail {
template <auto...>
struct AlwaysFalse {
  static constexpr bool value = false;
};
}  // namespace detail

namespace df {
template <typename... Ts>
class DataFrame {
 public:
  static constexpr int NumCols = sizeof...(Ts);
  using RowType = std::tuple<Ts...>;
  using RefType = std::tuple<Ts&...>;
  using ConstRefType = std::tuple<Ts const&...>;

  using Iterator = DataFrameIterator<DataFrame>;
  using ConstIterator = DataFrameConstIterator<DataFrame>;

 private:
  // Helper functions to retrieve each column vector type.
  template <std::size_t Col>
  auto getCol() const -> auto{
    static_assert(detail::AlwaysFalse<Col>::value, "Method cannot be called");
    return std::get<Col>(columns_);
  }

  template <std::size_t Col>
  using ColType = std::remove_reference_t<
      decltype(std::declval<DataFrame>().template getCol<Col>())>;

 public:
  template <std::size_t Col>
  using ColIterator = typename ColType<Col>::iterator;

  template <std::size_t Col>
  using ConstColIterator = typename ColType<Col>::const_iterator;

  constexpr DataFrame() noexcept = default;

  ~DataFrame() noexcept = default;

  constexpr DataFrame(DataFrame const& df) noexcept {
    int newCapacity = std::get<0>(df.columns_).capacity();
    impl::ReserveColumns(columns_, newCapacity,
                         std::make_index_sequence<NumCols>{});
    std::cout << "NumCols = " << NumCols << "\n";
    // impl::CopyColumns(df.columns_, columns_, std::index_sequence<NumCols>{});
    for (int i = 0; i < df.size(); ++i) {
      append(df.get(i));
    }
  }

  constexpr DataFrame(DataFrame<Ts...>&& df) {
    if (this == &df) {
      return;
    }
    if (std::get<0>(columns_).capacity() <
        std::get<0>(columns_).size() + std::get<0>(df.columns_).size()) {
      int numElementsAfterAppend =
          std::get<0>(columns_).size() + std::get<0>(df.columns_).size();
      int newCapacity =
          1 << static_cast<int>(std::ceil(std::log2(numElementsAfterAppend)));
      impl::ReserveColumns(columns_, newCapacity,
                           std::make_index_sequence<NumCols>{});
    }
    impl::MoveColumns(std::move(df.columns_), columns_,
                      std::make_index_sequence<NumCols>{});
  }

  constexpr auto begin() noexcept -> Iterator { return {this, 0}; }

  constexpr auto cbegin() const noexcept -> ConstIterator { return {this, 0}; }

  constexpr auto end() noexcept -> Iterator { return {this, size()}; }

  constexpr auto cend() const noexcept -> ConstIterator {
    return {this, size()};
  }

  constexpr auto size() const noexcept -> int {
    return std::get<0>(columns_).size();
  }

  constexpr auto capacity() const noexcept -> int {
    return std::get<0>(columns_).capacity();
  }

  constexpr auto reserve(std::size_t newCapacity) noexcept -> void {
    impl::ReserveColumns(columns_, newCapacity,
                         std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(Ts const&... em) noexcept -> void {
    impl::Append(columns_, std::tie(em...),
                 std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(Ts&&... em) noexcept -> void {
    impl::Append(columns_, std::tuple(std::forward<Ts>(em)...),
                 std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(std::tuple<Ts...> const& em) -> void {
    impl::Append(columns_, em, std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(std::tuple<Ts...>&& em) -> void {
    impl::Append(columns_, std::forward<std::tuple<Ts...>>(em),
                 std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(DataFrame<Ts...> const& df) -> void {
    if (std::get<0>(columns_).capacity() <
        std::get<0>(columns_).size() + std::get<0>(df.columns_).size()) {
      int numElementsAfterAppend =
          std::get<0>(columns_).size() + std::get<0>(df.columns_).size();
      int newCapacity =
          1 << static_cast<int>(std::ceil(std::log2(numElementsAfterAppend)));
      impl::ReserveColumns(columns_, newCapacity,
                           std::make_index_sequence<NumCols>{});
    }
    // impl::CopyColumns(df.columns_, columns_, std::index_sequence<NumCols>{});
    for (int i = 0; i < df.size(); ++i) {
      append(df.get(i));
    }
  }

  constexpr auto append(DataFrame<Ts...>&& df) -> void {
    if (this == &df) {
      return;
    }
    if (std::get<0>(columns_).capacity() <
        std::get<0>(columns_).size() + std::get<0>(df.columns_).size()) {
      int numElementsAfterAppend =
          std::get<0>(columns_).size() + std::get<0>(df.columns_).size();
      int newCapacity =
          1 << static_cast<int>(std::ceil(std::log2(numElementsAfterAppend)));
      impl::ReserveColumns(columns_, newCapacity,
                           std::make_index_sequence<NumCols>{});
    }
    impl::MoveAppend(std::move(df.columns_), columns_,
                     std::make_index_sequence<NumCols>{});
  }

  constexpr auto get(int row) noexcept -> RefType {
    return impl::Get(columns_, row, std::make_index_sequence<NumCols>{});
  }

  constexpr auto get(int row) const noexcept -> ConstRefType {
    return impl::Get(columns_, row, std::make_index_sequence<NumCols>{});
  }

  template <int ColNum>
  auto printCol() const noexcept
      -> std::enable_if_t<(0 <= ColNum) && (ColNum < NumCols)> {
    std::cout << "Column " << ColNum << "\n";
    std::cout << "Num of elements: " << std::get<ColNum>(columns_).size()
              << "\n";
    std::cout << "Elements: ";
    for (auto const& em : std::get<ColNum>(columns_)) {
      std::cout << em << " ";
    }
    std::cout << "\n";
  }

 private:
  std::tuple<std::vector<Ts>...> columns_;
};
}  // namespace df
