#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace limits
{
  
class TestBase
{
public:
  TestBase(const char *name)
  : m_name(name)
  {
  }
  
  const std::string &name() const { return m_name; }
  
  virtual void run() = 0;
  
private:
  std::string m_name;
};
  
class Tests
{
public:
  Tests(int argc, char **argv)
  {
  }
  
  static void install(TestBase *t)
  {
    m_tests.push_back(t);
  }
  
  int run()
  {
    std::cout << "Running all tests" << std::endl;
    bool all_success = true;
  
    for (auto &t : m_tests)
    {
      try
      {
        t->run();
      }
      catch(...)
      {
        std::cerr << "Failed to run test " << t->name();
        all_success = false;
      }
    }
  
    std::cout << "... tests complete" << std::endl;
    return all_success ? 0 : 1;
  }
  
  static std::vector<TestBase *> m_tests;
};

}

#define LIMITS_TEST_NAME() Test##__LINE__
#define LIMITS_TEST(name) \
  class LIMITS_TEST_NAME() : \
    public limits::TestBase { \
      LIMITS_TEST_NAME()() : TestBase(#name) { limits::Tests::install(this); } \
      void run() override; }; \
  LIMITS_TEST_NAME() g_##LIMITS_TEST_NAME(); \
  void LIMITS_TEST_NAME()::run()

#define LIMITS_APP() int main(int argc, char **argv) { limits::Tests tests(argc, argv); return tests.run(); }
