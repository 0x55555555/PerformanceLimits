#include "Limits.h"
#include <array>
#include <cmath>

LIMITS_TEST(memcpy, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  using Megabyte = std::array<char, 1000 * 1000>;
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  run([&]() {
    memcpy(b.data(), a.data(), a.size() * sizeof(*a.data()));
  });
}

LIMITS_TEST(multiply, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  std::vector<int> c(p.count);
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      c[i] = a[i] * b[i];
    }
  });
}

LIMITS_TEST(complex, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  std::vector<int> c(p.count);
  std::vector<int> d1(p.count);
  std::vector<int> d2(p.count);
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      auto a_ = a[i];
      auto b_ = b[i];
      auto c_ = c[i];
      
      d1[i] = (-b_ + (b_ * b_ - 4 * a_ * c_)) / 2 * a_;
      d2[i] = (-b_ - (b_ * b_ - 4 * a_ * c_)) / 2 * a_;
    }
  });
}

LIMITS_TEST(sqrt, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  std::vector<int> c(p.count);
  std::vector<int> d1(p.count);
  std::vector<int> d2(p.count);
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      auto a_ = a[i];
      auto b_ = b[i];
      auto c_ = c[i];
      
      d1[i] = (-b_ + sqrt(b_ * b_ - 4 * a_ * c_)) / 2 * a_;
      d2[i] = (-b_ - sqrt(b_ * b_ - 4 * a_ * c_)) / 2 * a_;
    }
  });
}

LIMITS_APP()
