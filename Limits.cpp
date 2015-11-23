#include "Limits.h"

namespace limits
{
  
std::vector<TestBase *> &tests_vec()
{
  static std::vector<TestBase *> tests;
  return tests;
}

  
const std::vector<TestBase *> &Tests::tests()
{
  return tests_vec();
}

void Tests::install(TestBase *t)
{
  tests_vec().push_back(t);
}

void TestBase::prevent_optimisation(const void *data, std::size_t i)
{
  (void)data;
}
}

#include <sstream>

extern "C" {
const char *runPerformanceTests()
{
  static std::string output;
  limits::Tests tests(0, nullptr);
  std::stringstream str;
  tests.run(str);
  output = str.str();
  return output.data();
}
}