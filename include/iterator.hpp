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

#include <algorithm>

#include "dataframe_fwd.hpp"

namespace df {
template <typename DF>
class DataFrameIterator {
 public:
  constexpr DataFrameIterator(DF* df, int pos = 0) noexcept : df_{df}, pos_{pos} {}

  ~DataFrameIterator() = default;

  constexpr DataFrameIterator(DataFrameIterator const& it) noexcept
      : df_{it.df_}, pos_{it.pos_} {}

  constexpr auto operator=(DataFrameIterator const& it) noexcept -> DataFrameIterator& {
    df_ = it.df_;
    pos_ = it.pos_;
    return *this;
  }

  constexpr auto operator++() noexcept -> DataFrameIterator& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  constexpr auto operator++(int) noexcept -> DataFrameIterator& {
    DataFrameIterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  constexpr auto operator--() noexcept -> DataFrameIterator& {
    if (pos_ < df_->size()) {
      --pos_;
    }
    return *this;
  }

  constexpr auto operator--(int) noexcept -> DataFrameIterator& {
    DataFrameIterator ret = *this;
    if (pos_ < df_->size()) {
      --pos_;
    }
    return ret;
  }

  constexpr auto operator+(int offset) const noexcept -> DataFrameIterator {
    DataFrameIterator ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ + offset), df_->size());
    return ret;
  }

  constexpr auto operator-(int offset) const noexcept -> DataFrameIterator {
    DataFrameIterator ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ - offset), df_->size());
    return ret;
  }

  constexpr auto operator+=(int offset) noexcept -> DataFrameIterator& {
    pos_ = std::min(std::max(0, pos_ + offset), df_->size());
    return *this;
  }

  constexpr auto operator-=(int offset) noexcept -> DataFrameIterator& {
    pos_ = std::min(std::max(0, pos_ - offset), df_->size());
    return *this;
  }

  constexpr auto operator==(DataFrameIterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  constexpr auto operator!=(DataFrameIterator const& it) const noexcept -> bool {
    return !(*this == it);
  }

  constexpr auto operator*() noexcept -> typename DF::RefType { return df_->get(pos_); }

  constexpr auto operator*() const noexcept -> typename DF::ConstRefType {
    return static_cast<DF const*>(df_)->get(pos_);
  }

  constexpr auto operator->() noexcept -> typename DF::RefType* {
    return &df_->get(pos_);
  }

  constexpr auto operator->() const noexcept -> typename DF::ConstRefType const* {
    return &static_cast<DF const*>(df_)->get(pos_);
  }

 private:
  DF* df_;
  int pos_;
};

template <typename DF>
class DataFrameConstIterator {
 public:
  DataFrameConstIterator(const DF* df, int pos = 0) noexcept
      : df_{df}, pos_{pos} {}

  ~DataFrameConstIterator() = default;

  DataFrameConstIterator(DataFrameConstIterator const& it) noexcept
      : df_{it.df_}, pos_{it.pos_} {}

  constexpr auto operator=(DataFrameConstIterator const& it) noexcept -> DataFrameConstIterator& {
    df_ = it.df_;
    pos_ = it.pos_;
    return *this;
  }

  constexpr auto operator++() noexcept -> DataFrameConstIterator& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  constexpr auto operator++(int) noexcept -> DataFrameConstIterator& {
    DataFrameConstIterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  constexpr auto operator--() noexcept -> DataFrameConstIterator& {
    if (pos_ > 0) {
      --pos_;
    }
    return *this;
  }

  constexpr auto operator--(int) noexcept -> DataFrameConstIterator& {
    DataFrameConstIterator ret = *this;
    if (pos_ > 0) {
      --pos_;
    }
    return ret;
  }

  constexpr auto operator+(int offset) const noexcept -> DataFrameConstIterator {
    DataFrameConstIterator ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ + offset), df_->size());
    return ret;
  }

  constexpr auto operator-(int offset) noexcept -> DataFrameConstIterator {
    DataFrameConstIterator ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ - offset), df_->size());
    return ret;
  }

  constexpr auto operator+=(int offset) noexcept -> DataFrameConstIterator& {
    pos_ = std::min(std::max(0, pos_ + offset), df_->size());
    return *this;
  }

  constexpr auto operator-=(int offset) noexcept -> DataFrameConstIterator& {
    pos_ = std::min(std::max(0, pos_ - offset), df_->size());
    return *this;
  }

  constexpr auto operator==(DataFrameConstIterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  constexpr auto operator!=(DataFrameConstIterator const& it) const noexcept -> bool {
    return !(*this == it);
  }

  constexpr auto operator*() const noexcept -> typename DF::ConstRefType {
    return df_->get(pos_);
  }

  constexpr auto operator->() const noexcept -> typename DF::ConstRefType const* {
    return &static_cast<DF const*>(df_)->get(pos_);
  }

 private:
  DF const* df_;
  int pos_;
};
}  // namespace df
