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
class RowIteratorImpl {
 public:
  constexpr RowIteratorImpl(DF* df, int pos = 0) noexcept
      : df_{df}, pos_{pos} {}

  ~RowIteratorImpl() = default;

  constexpr RowIteratorImpl(RowIteratorImpl const& it) noexcept
      : df_{it.df_}, pos_{it.pos_} {}

  constexpr auto operator=(RowIteratorImpl const& it) noexcept
      -> RowIteratorImpl& {
    df_ = it.df_;
    pos_ = it.pos_;
    return *this;
  }

  constexpr auto operator++() noexcept -> RowIteratorImpl& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  constexpr auto operator++(int) noexcept -> RowIteratorImpl& {
    RowIteratorImpl ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  constexpr auto operator--() noexcept -> RowIteratorImpl& {
    if (pos_ < df_->size()) {
      --pos_;
    }
    return *this;
  }

  constexpr auto operator--(int) noexcept -> RowIteratorImpl& {
    RowIteratorImpl ret = *this;
    if (pos_ < df_->size()) {
      --pos_;
    }
    return ret;
  }

  constexpr auto operator+(int offset) const noexcept -> RowIteratorImpl {
    RowIteratorImpl ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ + offset), df_->size());
    return ret;
  }

  constexpr auto operator-(int offset) const noexcept -> RowIteratorImpl {
    RowIteratorImpl ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ - offset), df_->size());
    return ret;
  }

  constexpr auto operator+=(int offset) noexcept -> RowIteratorImpl& {
    pos_ = std::min(std::max(0, pos_ + offset), df_->size());
    return *this;
  }

  constexpr auto operator-=(int offset) noexcept -> RowIteratorImpl& {
    pos_ = std::min(std::max(0, pos_ - offset), df_->size());
    return *this;
  }

  constexpr auto operator==(RowIteratorImpl const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  constexpr auto operator!=(RowIteratorImpl const& it) const noexcept -> bool {
    return !(*this == it);
  }

  constexpr auto operator*() noexcept -> typename DF::RefType {
    return df_->get(pos_);
  }

  constexpr auto operator*() const noexcept -> typename DF::ConstRefType {
    return static_cast<DF const*>(df_)->get(pos_);
  }

  constexpr auto operator->() noexcept -> typename DF::RefType* {
    return &df_->get(pos_);
  }

  constexpr auto operator->() const noexcept ->
      typename DF::ConstRefType const* {
    return &static_cast<DF const*>(df_)->get(pos_);
  }

 private:
  DF* df_;
  int pos_;
};

template <typename DF>
class ConstRowIteratorImpl {
 public:
  ConstRowIteratorImpl(const DF* df, int pos = 0) noexcept
      : df_{df}, pos_{pos} {}

  ~ConstRowIteratorImpl() = default;

  ConstRowIteratorImpl(ConstRowIteratorImpl const& it) noexcept
      : df_{it.df_}, pos_{it.pos_} {}

  constexpr auto operator=(ConstRowIteratorImpl const& it) noexcept
      -> ConstRowIteratorImpl& {
    df_ = it.df_;
    pos_ = it.pos_;
    return *this;
  }

  constexpr auto operator++() noexcept -> ConstRowIteratorImpl& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  constexpr auto operator++(int) noexcept -> ConstRowIteratorImpl& {
    ConstRowIteratorImpl ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  constexpr auto operator--() noexcept -> ConstRowIteratorImpl& {
    if (pos_ > 0) {
      --pos_;
    }
    return *this;
  }

  constexpr auto operator--(int) noexcept -> ConstRowIteratorImpl& {
    ConstRowIteratorImpl ret = *this;
    if (pos_ > 0) {
      --pos_;
    }
    return ret;
  }

  constexpr auto operator+(int offset) const noexcept -> ConstRowIteratorImpl {
    ConstRowIteratorImpl ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ + offset), df_->size());
    return ret;
  }

  constexpr auto operator-(int offset) noexcept -> ConstRowIteratorImpl {
    ConstRowIteratorImpl ret = *this;
    ret.pos_ = std::min(std::max(0, ret.pos_ - offset), df_->size());
    return ret;
  }

  constexpr auto operator+=(int offset) noexcept -> ConstRowIteratorImpl& {
    pos_ = std::min(std::max(0, pos_ + offset), df_->size());
    return *this;
  }

  constexpr auto operator-=(int offset) noexcept -> ConstRowIteratorImpl& {
    pos_ = std::min(std::max(0, pos_ - offset), df_->size());
    return *this;
  }

  constexpr auto operator==(ConstRowIteratorImpl const& it) const noexcept
      -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  constexpr auto operator!=(ConstRowIteratorImpl const& it) const noexcept
      -> bool {
    return !(*this == it);
  }

  constexpr auto operator*() const noexcept -> typename DF::ConstRefType {
    return df_->get(pos_);
  }

  constexpr auto operator->() const noexcept ->
      typename DF::ConstRefType const* {
    return &static_cast<DF const*>(df_)->get(pos_);
  }

 private:
  DF const* df_;
  int pos_;
};
}  // namespace df
