#include "Limits.h"
#include <array>
#include <cmath>
#include <future>

using Megabyte = std::array<char, 1000 * 1000>;

LIMITS_TEST(memfill, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  std::size_t size = (sizeof(Megabyte) * p.count) / sizeof(std::int32_t);
  
  std::vector<std::int32_t> a(size);
  
  run([&]() {
    std::fill(a.begin(), a.end(), 55);
  });
}

LIMITS_TEST(memset, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  std::size_t size = (sizeof(Megabyte) * p.count) / sizeof(std::int32_t);
  
  std::vector<std::int32_t> a(size);
  
  run([&]() {
    memset(a.data(), 55, a.size() * sizeof(a[0]));
  });
}

LIMITS_TEST(memcpy, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data());
  
  run([&]() {
    memcpy(b.data(), a.data(), size);
  });
}

LIMITS_TEST(memmove, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data());
  
  run([&]() {
    memmove(b.data(), a.data(), size);
  });
}

LIMITS_TEST(memcpy_threaded_2, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
  std::vector<Megabyte> a(p.count);
  std::vector<Megabyte> b(p.count);
  
  std::size_t size = a.size() * sizeof(*a.data()) / 2;
  
  run([&]() {
    auto fut_a = std::async(std::launch::async, [&]() { memcpy((char*)b.data(), (char*)a.data(), size); });
    auto fut_b = std::async(std::launch::async, [&]() { memcpy((char*)b.data() + size, (char*)a.data() + size, size); });
    
    fut_a.get();
    fut_b.get();
    
    prevent_optimisation(b);
  });
}

LIMITS_TEST(memcpy_threaded_4, Sized) {
  
  auto p = parameters();
  setup().unit = "MB";
  
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
    
    prevent_optimisation(b);
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
    prevent_optimisation(c);
  });
}

LIMITS_TEST(square, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      b[i] = a[i] * a[i];
    }
    prevent_optimisation(b);
  });
}

LIMITS_TEST(assign, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  
  run([&]() {
    for (std::size_t i = 0; i < a.size(); ++i) {
      b[i] = a[i];
    }
    prevent_optimisation(b);
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
    prevent_optimisation(b);
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
    prevent_optimisation(d1);
    prevent_optimisation(d2);
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
    prevent_optimisation(d1);
    prevent_optimisation(d2);
  });
}

LIMITS_TEST(quadratic_threaded_8, Count) {
  
  auto p = parameters();
  setup().unit = "operation";
  
  std::vector<int> a(p.count);
  std::vector<int> b(p.count);
  std::vector<int> c(p.count);
  std::vector<int> d1(p.count);
  std::vector<int> d2(p.count);
  
  auto solve = [&](std::size_t start, std::size_t length) {
    for (std::size_t i = start; i < length; ++i) {
      auto a_ = a[i];
      auto b_ = b[i];
      auto c_ = c[i];
      
      d1[i] = (-b_ + sqrt(b_ * b_ - 4 * a_ * c_)) / 2 * a_;
      d2[i] = (-b_ - sqrt(b_ * b_ - 4 * a_ * c_)) / 2 * a_;
    }
  };
  
  run([&]() {
    std::size_t section = a.size() / 8;
    auto fut_1 = std::async(std::launch::async, [&]() { solve(section*0, section); });
    auto fut_2 = std::async(std::launch::async, [&]() { solve(section*1, section); });
    auto fut_3 = std::async(std::launch::async, [&]() { solve(section*2, section); });
    auto fut_4 = std::async(std::launch::async, [&]() { solve(section*3, section); });
    auto fut_5 = std::async(std::launch::async, [&]() { solve(section*4, section); });
    auto fut_6 = std::async(std::launch::async, [&]() { solve(section*5, section); });
    auto fut_7 = std::async(std::launch::async, [&]() { solve(section*6, section); });
    auto fut_8 = std::async(std::launch::async, [&]() { solve(section*7, section); });
    
    fut_1.get(); fut_2.get(); fut_3.get(); fut_4.get(); fut_5.get(); fut_6.get(); fut_7.get(); fut_8.get();
    
    prevent_optimisation(d1);
    prevent_optimisation(d2);
  });
}

LIMITS_APP()
