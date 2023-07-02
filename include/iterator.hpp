#pragma once

#include "dataframe.hpp"


template <typename... Ts>
class DataFrame<Ts...>::Iterator {
 public:
  Iterator(DataFrame<Ts...>* df, int pos = 0) : df_{df}, pos_{pos} {}

  Iterator(Iterator const& it) : df_{it.df_}, pos_{it.pos_} {}

  Iterator& operator++() {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  Iterator& operator++(int) {
    Iterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  auto operator--() -> Iterator& {
    if (pos_ < df_->size()) {
      --pos_;
    }
    return *this;
  }

  auto operator--(int) -> Iterator& {
    Iterator ret = *this;
    if (pos_ < df_->size()) {
      --pos_;
    }
    return ret;
  }

  auto operator==(Iterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  auto operator!=(Iterator const& it) const noexcept -> bool {
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
class DataFrame<Ts...>::ConstIterator {
 public:
  ConstIterator(const DataFrame<Ts...>* df, int pos = 0) : df_{df}, pos_{pos} {}

  ConstIterator(Iterator const& it) : df_{it.df_}, pos_{it.pos_} {}

  auto operator++() -> ConstIterator& {
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return *this;
  }

  auto operator++(int) -> ConstIterator& {
    ConstIterator ret = *this;
    if (pos_ < df_->size()) {
      ++pos_;
    }
    return ret;
  }

  auto operator--() -> ConstIterator& {
    if (pos_ > 0) {
      --pos_;
    }
    return *this;
  }

  auto operator--(int) -> ConstIterator& {
    ConstIterator ret = *this;
    if (pos_ > 0) {
      --pos_;
    }
    return ret;
  }

  auto operator==(ConstIterator const& it) const noexcept -> bool {
    return (this == &it) || (df_ == it.df_ && pos_ == it.pos_);
  }

  auto operator!=(ConstIterator const& it) const noexcept -> bool {
    return !(*this == it);
  }

  auto operator*() const noexcept -> std::tuple<Ts const&...> {
    return df_->get(pos_);
  }

 private:
  DataFrame<Ts...> const* df_;
  int pos_;
};
