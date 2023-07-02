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

#include <functional>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

namespace df {
namespace impl {
/**
 * Append an element to the columns.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto Append(std::tuple<std::vector<Ts>...>& columns,
                      std::tuple<Ts const&...> const& em,
                      std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "Append() with std::tuple<Ts const&...> const&\n";
  (std::get<Is>(columns).push_back(std::get<Is>(em)), ...);
}

/**
 * Append an element to the columns.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto Append(std::tuple<std::vector<Ts>...>& columns,
                      std::tuple<Ts&&...>& em,
                      std::index_sequence<Is...>) noexcept -> void {
  std::cout << "Append() with std::tuple<Ts&&...>&\n";
  (std::get<Is>(columns).emplace_back(std::move(std::get<Is>(em))), ...);
}

/**
 * Append an element to the columns using move semantics.
 *
 * @param columns The columns of the DF.
 * @param em The new element.
 */
template <typename... Ts, std::size_t... Is>
constexpr auto Append(std::tuple<std::vector<Ts>...>& columns,
                      std::tuple<Ts...>&& em,
                      std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "Append() with std::tuple<Ts...>&&\n";
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
constexpr auto Get(std::tuple<std::vector<Ts>...>& columns, int row,
                   std::index_sequence<Is...>) noexcept -> std::tuple<Ts&...> {
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
constexpr auto Get(std::tuple<std::vector<Ts>...> const& columns, int row,
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
constexpr auto CopyColumns(std::tuple<std::vector<Ts> const&...>& colFrom,
                           std::tuple<std::vector<Ts>&...>& colTo,
                           std::index_sequence<Is...>) noexcept -> void {
  // std::cout << "CopyColumns()\n";
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
constexpr auto MoveColumns(std::tuple<std::vector<Ts>...>&& colFrom,
                           std::tuple<std::vector<Ts>...>& colTo,
                           std::index_sequence<Is...>) noexcept -> void {
  std::cout << "MoveColumns()\n";
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
constexpr auto ReserveColumns(std::tuple<std::vector<Ts>...>& columns,
                              std::size_t newCapacity,
                              std::index_sequence<Is...>) -> void {
  ((std::get<Is>(columns).reserve(newCapacity)), ...);
}
}  // namespace impl
}  // namespace df
