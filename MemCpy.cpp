#include "Limits.h"
#include <array>
#include <cmath>
#include <future>

LIMITS_TEST(memcpy, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  using Megabyte = std::array<char, 1000 * 1000>;
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data());
  
  run([&]() {
    memcpy(b.data(), a.data(), size);
  });
}

LIMITS_TEST(memcpy_threaded_2, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  using Megabyte = std::array<char, 1000 * 1000>;
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data()) / 2;
  
  run([&]() {
    auto fut_a = std::async(std::launch::async, [&]() { memcpy((char*)b.data(), (char*)a.data(), size); });
    auto fut_b = std::async(std::launch::async, [&]() { memcpy((char*)b.data() + size, (char*)a.data() + size, size); });
    
    fut_a.get();
    fut_b.get();
  });
}

LIMITS_TEST(memcpy_threaded_4, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  using Megabyte = std::array<char, 1000 * 1000>;
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data()) / 4;
  
  run([&]() {
    auto fut_a = std::async(std::launch::async, [&]() { memcpy((char*)b.data(), (char*)a.data(), size); });
    auto fut_b = std::async(std::launch::async, [&]() { memcpy((char*)b.data() + size, (char*)a.data() + size, size); });
    auto fut_c = std::async(std::launch::async, [&]() { memcpy((char*)b.data() + size*2, (char*)a.data() + size*2, size); });
    auto fut_d = std::async(std::launch::async, [&]() { memcpy((char*)b.data() + size*3, (char*)a.data() + size*3, size); });
    
    fut_a.get();
    fut_b.get();
    fut_c.get();
    fut_d.get();
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
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      b[i] = sqrt(a[i]);
    }
  });
}

LIMITS_TEST(quadratic, Count) {
  
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
