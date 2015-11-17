#include "Limits.h"
#include <array>

LIMITS_TEST(thing, Sized) {
  
  auto p = parameters();
  
  Setup setup;
  setup.unit = "MB";
  
  using Megabyte = std::array<char, 1000 * 1000>;
  
  std::vector<Megabyte> a(p.size);
  std::vector<Megabyte> b(p.size);
  
  run(setup, [&](std::size_t size, const auto &time) {
    memcpy(b.data(), a.data(), a.size() * sizeof(*a.data()));
  });
}

LIMITS_APP()
