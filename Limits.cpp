#include "Limits.h"

namespace limits
{
  
std::vector<TestBase *> Tests::m_tests;

void TestBase::prevent_optimisation(const void *data, std::size_t i)
{
  (void)data;
}
}