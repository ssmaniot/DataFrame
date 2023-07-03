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

  using RowIterator = RowIteratorImpl<DataFrame>;
  using ConstRowIterator = ConstRowIteratorImpl<DataFrame>;

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

  /**
   * @brief Default constructor. Create an empty DF.
   */
  constexpr DataFrame() noexcept = default;

  /**
   * @brief Default destructor.
   */
  ~DataFrame() noexcept = default;

  /**
   * @brief Copy constructor.
   *
   * @param df The DF from which the data is copied from.
   */
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

  /**
   * @brief Move constructor. Clears data from @df.
   *
   * @param df The DF from which the data is moved from.
   */
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

  /**
   * @brief Returns a read/write iterator that points to the
   * first row of the data.
   *
   * @return A read/write row iterator.
   */
  constexpr auto begin() noexcept -> RowIterator { return {this, 0}; }

  /**
   * @brief Returns a read-only iterator that points to the first row of the
   * data. Iteration is done in ordinary element order.
   *
   * @return A read-only row iterator pointing to the first row.
   */
  constexpr auto cbegin() const noexcept -> ConstRowIterator {
    return {this, 0};
  }

  /**
   * @brief Returns a read/write iterator that points one past the last
   * row of the data. Iteration is done in ordinary element order.
   *
   * @return A read/write row iterator pointing one past the last row.
   */
  constexpr auto end() noexcept -> RowIterator { return {this, size()}; }

  /**
   * @brief Returns a read-only iterator that points one past the last
   * row of the data. Iteration is done in ordinary element order.
   *
   * @return A read-only row iterator pointing one past the last row.
   */
  constexpr auto cend() const noexcept -> ConstRowIterator {
    return {this, size()};
  }

  /**
   * @brief Returns the number of rows stored in the %DataFrame.
   */
  constexpr auto size() const noexcept -> int {
    return std::get<0>(columns_).size();
  }

  /**
   * @brief Returns the total number of elements that the %DataFrame can hold
   * before needing to allocate more memory.
   */
  constexpr auto capacity() const noexcept -> int {
    return std::get<0>(columns_).capacity();
  }

  /**
   * @brief Reallocate memory to resize the total number of elements that the
   * %DataFrame can hold before needing to allocate more memory.
   */
  constexpr auto reserve(std::size_t newCapacity) noexcept -> void {
    impl::ReserveColumns(columns_, newCapacity,
                         std::make_index_sequence<NumCols>{});
  }

  /**
   *  @brief Append a row to the end of the %DataFrame.
   *  @param em Data to be added.
   */
  constexpr auto append(Ts const&... em) noexcept -> void {
    impl::Append(columns_, std::tie(em...),
                 std::make_index_sequence<NumCols>{});
  }

  /**
   *  @brief Append a row to the end of the %DataFrame.
   *  @param em Data to be added.
   */
  constexpr auto append(Ts&&... em) noexcept -> void {
    impl::Append(columns_, std::tuple(std::forward<Ts>(em)...),
                 std::make_index_sequence<NumCols>{});
  }

  /**
   *  @brief Append a row to the end of the %DataFrame.
   *  @param em Data to be added.
   */
  constexpr auto append(std::tuple<Ts...> const& em) -> void {
    impl::Append(columns_, em, std::make_index_sequence<NumCols>{});
  }

  /**
   *  @brief Append a row to the end of the %DataFrame.
   *  @param em Data to be added.
   */
  constexpr auto append(std::tuple<Ts...>&& em) -> void {
    impl::Append(columns_, std::forward<std::tuple<Ts...>>(em),
                 std::make_index_sequence<NumCols>{});
  }

  /**
   *  @brief Append the data in @df to the end of the %DataFrame by copy.
   *  @param df %DataFrame whose data is to be appended to %DataFrame.
   */
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

  /**
   *  @brief Append the data in @df to the end of the %DataFrame by move.
   *  @param df %DataFrame whose data is to be move-appended to %DataFrame.
   */
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

  /**
   * @brief Get a reference to the elements of @row.
   */
  constexpr auto get(int row) noexcept -> RefType {
    return impl::Get(columns_, row, std::make_index_sequence<NumCols>{});
  }

  /**
   * @brief Get a const reference to the elements of @row.
   */
  constexpr auto get(int row) const noexcept -> ConstRefType {
    return impl::Get(columns_, row, std::make_index_sequence<NumCols>{});
  }

  /**
   * @brief Print out the column number, its number of elements, and its
   * elements..
   */
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
