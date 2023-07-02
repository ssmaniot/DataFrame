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

#include "dataframe_fwd.hpp"

namespace df {
template <typename... Ts>
class DataFrameIterator {
 public:
  DataFrameIterator(DataFrame<Ts...>* df, int pos = 0) : df_{df}, pos_{pos} {}

  DataFrameIterator(DataFrameIterator const& it) : df_{it.df_}, pos_{it.pos_} {}

  DataFrameIterator& operator++() {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  DataFrameIterator& operator++(int) {
    DataFrameIterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  auto operator--() -> DataFrameIterator& {
    if (pos_ < df_->size()) {
      --pos_;
    }
    return *this;
  }

  auto operator--(int) -> DataFrameIterator& {
    DataFrameIterator ret = *this;
    if (pos_ < df_->size()) {
      --pos_;
    }
    return ret;
  }

  auto operator==(DataFrameIterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  auto operator!=(DataFrameIterator const& it) const noexcept -> bool {
    return !(*this == it);
  }

  auto operator*() noexcept -> std::tuple<Ts&...> { return df_->get(pos_); }

  auto operator*() const noexcept -> std::tuple<Ts const&...> {
    return static_cast<DataFrame<Ts...> const*>(df_)->get(pos_);
  }

 private:
  DataFrame<Ts...>* df_;
  int pos_;
};

template <typename... Ts>
class DataFrameConstIterator {
 public:
  DataFrameConstIterator(const DataFrame<Ts...>* df, int pos = 0)
      : df_{df}, pos_{pos} {}

  DataFrameConstIterator(DataFrameConstIterator const& it)
      : df_{it.df_}, pos_{it.pos_} {}

  auto operator++() -> DataFrameConstIterator& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  auto operator++(int) -> DataFrameConstIterator& {
    DataFrameConstIterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  auto operator--() -> DataFrameConstIterator& {
    if (pos_ > 0) {
      --pos_;
    }
    return *this;
  }

  auto operator--(int) -> DataFrameConstIterator& {
    DataFrameConstIterator ret = *this;
    if (pos_ > 0) {
      --pos_;
    }
    return ret;
  }

  auto operator==(DataFrameConstIterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  auto operator!=(DataFrameConstIterator const& it) const noexcept -> bool {
    return !(*this == it);
  }

  auto operator*() const noexcept -> std::tuple<Ts const&...> {
    return df_->get(pos_);
  }

 private:
  DataFrame<Ts...> const* df_;
  int pos_;
};
}  // namespace df
