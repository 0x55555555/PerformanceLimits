#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace limits
{
  
class TestBase
{
public:
  enum Type
  {
    Sized
  };
  
  struct Parameters
  {
    std::size_t size;
  };
  
  struct Setup
  {
    std::string unit;
  };
  
  TestBase(const char *name, Type t)
  : m_name(name)
  , m_type(t)
  {
  }
  
  const std::string &name() const { return m_name; }
  
  void callback()
  {
    if (m_type == Sized) {
      m_parameters.size = 1;
    }
  
    callback_impl();
  }
  
  virtual void callback_impl() = 0;
  
  template <typename T> void run(const Setup &s, const T &t)
  {
  }
  
  const Parameters &parameters()
  {
    return m_parameters;
  }
  
private:
  std::string m_name;
  Type m_type;
  
  Parameters m_parameters;
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
      std::cout << "  Running test " << t->name() << std::endl;
      try
      {
        t->callback();
      }
      catch(...)
      {
        std::cerr << "    ... failed in test" << std::endl;
        all_success = false;
      }
    
      std::cout << "  ... complete" << std::endl;
    }
  
    std::cout << "... all tests complete" << std::endl;
    return all_success ? 0 : 1;
  }
  
  static std::vector<TestBase *> m_tests;
};

}

#define LIMITS_TEST_NAME() Test##__LINE__
#define LIMITS_TEST(name, type) \
  struct LIMITS_TEST_NAME() : \
    public limits::TestBase { \
      LIMITS_TEST_NAME()() : TestBase(#name, limits::TestBase::type) \
        { limits::Tests::install(this); } \
      void callback_impl() override; }; \
  LIMITS_TEST_NAME() g_##LIMITS_TEST_NAME; \
  void LIMITS_TEST_NAME()::callback_impl()

#define LIMITS_APP() int main(int argc, char **argv) { limits::Tests tests(argc, argv); return tests.run(); }
