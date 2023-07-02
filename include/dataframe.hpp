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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

#include "iterator.hpp"

namespace detail {
/**
 * Append an element to the columns.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto AppendImpl(std::tuple<std::vector<Ts>...>& columns,
                          std::tuple<Ts const&...> const& em,
                          std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "AppendImpl() with std::tuple<Ts const&...> const&\n";
  (std::get<Is>(columns).push_back(std::get<Is>(em)), ...);
}

/**
 * Append an element to the columns.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto AppendImpl(std::tuple<std::vector<Ts>...>& columns,
                          std::tuple<Ts&&...>& em,
                          std::index_sequence<Is...>) noexcept -> void {
  std::cout << "AppendImpl() with std::tuple<Ts&&...>&\n";
  (std::get<Is>(columns).emplace_back(std::move(std::get<Is>(em))), ...);
}

/**
 * Append an element to the columns using move semantics.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto AppendImpl(std::tuple<std::vector<Ts>...>& columns,
                          std::tuple<Ts...>&& em,
                          std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "AppendImpl() with std::tuple<Ts...>&&\n";
  (std::get<Is>(columns).emplace_back(std::move(std::get<Is>(em))), ...);
}

/**
 * Retrieve a row from the DF.
 *
 * @param columns The columns of the DF.
 * @param row The row number.
 * @returns A tuple of references to the row elements.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto GetImpl(std::tuple<std::vector<Ts>...>& columns, int row,
                       std::index_sequence<Is...>) noexcept
    -> std::tuple<Ts&...> {
  return std::tie(std::get<Is>(columns)[row]...);
}

/**
 * Retrieve a const row from the DF.
 *
 * @param columns The columns of the DF.
 * @param row The row number.
 * @returns A tuple of const references to the row elements.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto GetImpl(std::tuple<std::vector<Ts>...> const& columns, int row,
                       std::index_sequence<Is...>) noexcept
    -> std::tuple<Ts const&...> {
  return std::tie(std::get<Is>(columns)[row]...);
}

/**
 * Appends the data of @colFrom to @colTo by move.
 *
 * @param colFrom The columns of the DF to be moved.
 * @param colTo The columns of the DF to move to.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto MoveAppend(std::tuple<std::vector<Ts>...>&& colFrom,
                          std::tuple<std::vector<Ts>...>& colTo,
                          std::index_sequence<Is...>) noexcept -> void {
  ((std::move(std::begin(std::get<Is>(colFrom)),
              std::end(std::get<Is>(colFrom)),
              std::back_inserter(std::get<Is>(colTo))),
    std::get<Is>(colFrom).clear()),
   ...);
}

/**
 * Copy data from @colFrom to @colTo.
 *
 * @param colFrom The columns of the DF to copy from.
 * @param colTo The columns of the DF to copy into.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto CopyImpl(std::tuple<std::vector<Ts> const&...>& colFrom,
                        std::tuple<std::vector<Ts>&...>& colTo,
                        std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "CopyImpl()\n";
  (std::copy(std::begin(std::get<Is>(colFrom)), std::end(std::get<Is>(colFrom)),
             std::back_inserter(std::get<Is>(colTo))),
   ...);
  //((std::get<Is>(colTo) = std::move(std::get<Is>(colFrom)),
  //  std::get<Is>(colFrom).clear()),
  //  ...);
}

/**
 * Move data from @colFrom to @colTo.
 *
 * @param colFrom The columns of the DF to move.
 * @param colTo The columns of the DF to move to.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto MoveImpl(std::tuple<std::vector<Ts>...>&& colFrom,
                        std::tuple<std::vector<Ts>...>& colTo,
                        std::index_sequence<Is...>) noexcept -> void {
  std::cout << "MoveImpl()\n";
  ((std::get<Is>(colTo) = std::move(std::get<Is>(colFrom)),
    std::get<Is>(colFrom).clear()),
   ...);
}

/**
 * Reserves @newCapacity data to each element of @columns.
 *
 * @param columns Tuple of columns whose capacity is to be update.
 * @param newCapacity New capacity.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto ReserveAllColumns(std::tuple<std::vector<Ts>...>& columns,
                                 std::size_t newCapacity,
                                 std::index_sequence<Is...>) -> void {
  ((std::get<Is>(columns).reserve(newCapacity)), ...);
}
}  // namespace detail

template <typename... Ts>
class DataFrame {
 public:
  static constexpr int NumCols = sizeof...(Ts);
  using RowType = std::tuple<Ts...>;
  using RefType = std::tuple<Ts&...>;
  using ConstRefType = std::tuple<Ts const&...>;

  using Iterator = DataFrameIterator<Ts...>;
  using ConstIterator = DataFrameConstIterator<Ts...>;

  constexpr DataFrame() noexcept = default;

  ~DataFrame() noexcept = default;

  constexpr DataFrame(DataFrame const& df) noexcept {
    int newCapacity = std::get<0>(df.columns_).capacity();
    detail::ReserveAllColumns(columns_, newCapacity,
                              std::make_index_sequence<NumCols>{});
    std::cout << "NumCols = " << NumCols << "\n";
    // detail::CopyImpl(df.columns_, columns_, std::index_sequence<NumCols>{});
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
      detail::ReserveAllColumns(columns_, newCapacity,
                                std::make_index_sequence<NumCols>{});
    }
    detail::MoveImpl(std::move(df.columns_), columns_,
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
    detail::ReserveAllColumns(columns_, newCapacity,
                              std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(Ts const&... em) noexcept -> void {
    detail::AppendImpl(columns_, std::tie(em...),
                       std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(Ts&&... em) noexcept -> void {
    detail::AppendImpl(columns_, std::tuple(std::forward<Ts>(em)...),
                       std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(std::tuple<Ts...> const& em) -> void {
    detail::AppendImpl(columns_, em, std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(std::tuple<Ts...>&& em) -> void {
    detail::AppendImpl(columns_, std::forward<std::tuple<Ts...>>(em),
                       std::make_index_sequence<NumCols>{});
  }

  constexpr auto append(DataFrame<Ts...> const& df) -> void {
    if (std::get<0>(columns_).capacity() <
        std::get<0>(columns_).size() + std::get<0>(df.columns_).size()) {
      int numElementsAfterAppend =
          std::get<0>(columns_).size() + std::get<0>(df.columns_).size();
      int newCapacity =
          1 << static_cast<int>(std::ceil(std::log2(numElementsAfterAppend)));
      detail::ReserveAllColumns(columns_, newCapacity,
                                std::make_index_sequence<NumCols>{});
    }
    // detail::CopyImpl(df.columns_, columns_, std::index_sequence<NumCols>{});
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
      detail::ReserveAllColumns(columns_, newCapacity,
                                std::make_index_sequence<NumCols>{});
    }
    detail::MoveAppend(std::move(df.columns_), columns_,
                       std::make_index_sequence<NumCols>{});
  }

  constexpr auto get(int row) noexcept -> RefType {
    return detail::GetImpl(columns_, row, std::make_index_sequence<NumCols>{});
  }

  constexpr auto get(int row) const noexcept -> ConstRefType {
    return detail::GetImpl(columns_, row, std::make_index_sequence<NumCols>{});
  }

  template <int ColNum>
      auto printCol() const noexcept
      -> std::enable_if_t < 0 <= ColNum&& ColNum<NumCols, void> {
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
