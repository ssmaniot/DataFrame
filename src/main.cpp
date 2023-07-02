#include "dataframe.hpp"

#include <bits/utility.h>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

namespace {
  constexpr int NUM = 10'000'000;
  std::random_device rd{};
  std::mt19937 mt{rd()};
  std::uniform_int_distribution<int> ri(-1'000'000, 1'000'000);
  std::uniform_real_distribution<float> rf(-1'000'000.f, 1'000'000.f);
  std::uniform_int_distribution<char> rc('a', 'z');
}

template <typename TupleType, typename IS>
void PrintTupleImpl(TupleType const&, IS);

template <typename... Ts, std::size_t... Is>
void PrintTupleImpl(std::tuple<Ts...> const& data, std::index_sequence<Is...>) {
  static constexpr std::size_t lim = sizeof...(Ts);
  std::cout << "(";
  ((std::cout << std::get<Is>(data) << (Is < lim - 1 ? ", " : ")\n")), ...);
}

template <typename... Ts>
void PrintTuple(std::tuple<Ts...> const& data) {
  PrintTupleImpl(data, std::make_index_sequence<sizeof...(Ts)>{});
}

std::string ShortNumber(int n) {
  static std::vector<std::string> ord = {"", "K", "M", "B", "T", "Q"};
  std::string ret = "";
  int i = -1;
  do {
    ++i;
    if (n < 1'000) {
      ret = std::to_string(n);
    }
    n /= 1'000;
  } while  (n > 0);
  ret += ord[i];
  return ret;
}

std::string RandomString() {
  static std::uniform_int_distribution<int> sl(3, 40);
  std::string s = "";
  int len = sl(mt);
  for (int i = 0; i < len; ++i) {
    s += rc(mt);
  }
  return s;
}

class Int {
public:
  Int() noexcept: i{} { std::cout << "Int()\n"; }
  Int(int i) noexcept: i{i} { std::cout << "Int(int)\n"; }
  Int(Int const &o) noexcept { i = o.i; std::cout << "Int(Int const&)\n"; }
  Int(Int &&o) noexcept { i = o.i; std::cout << "Int(Int&&)\n"; }
  friend std::ostream& operator<<(std::ostream& os, Int const& i) {
    os << i.i;
    return os;
  }

private:
  int i;
};

class Double {
public:
  Double() noexcept: d{} { std::cout << "Double()\n"; }
  Double(double d) noexcept: d{d} { std::cout << "Double(double)\n"; }
  Double(Double const &o) noexcept { d = o.d; std::cout << "Double(Double const&)\n"; }
  Double(Double &&o) noexcept { d = o.d; std::cout << "Double(Double&&)\n"; }
  friend std::ostream& operator<<(std::ostream& os, Double const& d) {
    os << d.d;
    return os;
  }

private:
  double d;
};

int main(void) {
  std::cout << "DataFrame<int, std::string, double>, append + get\n";
  DataFrame<int, std::string, double> df;
  df.append(5, "Hello", 3.14);
  df.append(0, "World", 2.71);
  auto firstRow = df.get(0);
  std::cout << "(";
  std::cout << std::get<0>(firstRow) << ", ";
  std::cout << std::get<1>(firstRow) << ", ";
  std::cout << std::get<2>(firstRow) << " ";
  std::cout << ")\n";
  std::cout << "\n";

  std::cout << "const DataFrame<int, std::string, double>&, get\n";
  auto const& rdf = df;
  auto cr = rdf.get(1);
  std::cout << "(";
  std::cout << std::get<0>(cr) << ", ";
  std::cout << std::get<1>(cr) << ", ";
  std::cout << std::get<2>(cr);
  std::cout << ")\n";
  std::cout << "\n";

  std::cout << "DataFrame<Int, Double>, append (copy + move) + get\n";
  DataFrame<Int, Double> df2;
  Int i{5};
  Double d{2.};
  df2.append(i, d);
  df2.append(Int(-1), Double(7.12));
  auto fr = df2.get(0);
  std::cout << "(";
  std::cout << std::get<0>(fr) << ", ";
  std::cout << std::get<1>(fr);
  std::cout << ")\n";
  std::cout << "\n";

  std::cout << "DataFrame<int, double, char>, printCol + iterator + PrintTuple\n";
  DataFrame<int, double, char> df3;
  df3.append(0, 3.14, 'a');
  df3.append(1, 2.71, 'b');
  df3.append(2, 9.81, 'c');
  df3.printCol<0>();
  df3.printCol<1>();
  df3.printCol<2>();
  std::cout << "df3 printed using range-based loop:\n";
  for (auto const it : df3) {
    PrintTuple(it);
  }
  std::cout << "\n";

  std::cout << "DataFrame<int, double, char>, append move\n";
  DataFrame<int, double, char> df4;
  df4.append(3, 1.23, 'd');
  df4.append(4, 7.89, 'e');
  std::cout << "Size of df4 before move: " << df4.size() << "\n";
  std::cout << "df3 after append move:\n";
  df3.append(std::move(df4));
  for (auto const row : df3) {
    PrintTuple(row);
  }
  std::cout << "Size of df4 after move: " << df4.size() << "\n";
  std::cout << "\n";

  std::cout << "DataFrame<Int, Double>, append with copy and move, append df with copy and move\n";
  DataFrame<Int, Double> df5;
  df5.reserve(4);
  std::cout << "Append copy\n";
  df5.append(Int(0), Double(0.));
  std::cout << "\n";

  std::cout << "Append move\n";
  df5.append(Int(1), Double(1));
  std::cout << "\n";

  std::cout << "Append tuple copy\n";
  df5.append({Int(0), Double(0.)});
  std::cout << "\n";

  std::cout << "Construct tuple...\n";
  std::tuple t = {Int(1), Double(1)};
  std::cout << "\n";
  
  std::cout << "Append tuple move\n";
  df5.append(std::move(t));
  std::cout << "\n";

  DataFrame<Int, Double> df6;
  DataFrame<Int, Double> df7;
  df6.reserve(df5.capacity());
  df7.reserve(df5.capacity());
  
  std::cout << "Append df copy\n";
  df6.append(df5);
  std::cout << "\n";

  std::cout << "Append df move\n";
  df7.append(std::move(df5));
  std::cout << "\n";

  if (false)
  {
    std::cout << "Speed test, " << ShortNumber(NUM) << " elements (int, float)\n";
    DataFrame<int, float> df1;
    DataFrame<int, float> df2;
    std::cout << "Generate data...\n";
    std::vector<int> is;
    std::vector<float> fs;
    is.reserve(NUM);
    fs.reserve(NUM);
    for (int i = 0; i < NUM; ++i) {
      is.push_back(ri(mt));
      fs.push_back(rf(mt));
    }

    std::cout << "Insertion by copy...";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM; ++i) {
      df1.append(is[i], fs[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";
    
    std::cout << "Insertion by move...";
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM; ++i) {
      df2.append(std::move(is[i]), std::move(fs[i]));
    }
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    is.clear();
    fs.clear();
    std::cout << " Elapsed time: " << elapsed << " ms\n";

    DataFrame<int, float> df3;
    std::cout << "Append by copy...";
    start = std::chrono::high_resolution_clock::now();
    df3.append(df1);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";

    DataFrame<int, float> df4;
    std::cout << "Append by move...";
    start = std::chrono::high_resolution_clock::now();
    df4.append(std::move(df1));
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";
    std::cout << "\n";
  }

  if (false)
  {
    std::cout << "Speed test, " << ShortNumber(NUM) << " elements (string, string)\n";
    DataFrame<std::string, std::string> df1;
    DataFrame<std::string, std::string> df2;
    std::cout << "Generate data...\n";
    std::vector<std::string> is;
    std::vector<std::string> fs;
    is.reserve(NUM);
    fs.reserve(NUM);
    for (int i = 0; i < NUM; ++i) {
      is.push_back(RandomString());
      fs.push_back(RandomString());
    }

    std::cout << "Insertion by copy...";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM; ++i) {
      df1.append(is[i], fs[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";

    std::cout << "Insertion by move...";
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM; ++i) {
      df2.append(std::move(is[i]), std::move(fs[i]));
    }
    end = std::chrono::high_resolution_clock::now();
    is.clear();
    fs.clear();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";

    DataFrame<std::string, std::string> df3;
    std::cout << "Append by copy...";
    start = std::chrono::high_resolution_clock::now();
    df3.append(df1);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";

    DataFrame<std::string, std::string> df4;
    std::cout << "Append by move...";
    start = std::chrono::high_resolution_clock::now();
    df4.append(std::move(df1));
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << " Elapsed time: " << elapsed << " ms\n";
  }
}
