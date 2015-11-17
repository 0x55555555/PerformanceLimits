#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace limits
{
  
struct TestParameters
{
  std::size_t size;
};
  
inline std::ostream &operator<<(std::ostream &str, const TestParameters &p)
{
  return str << "{ " << p.size << " }";
}
  
class TestBase
{
public:
  enum Type
  {
    Sized
  };
  
  struct Setup
  {
    std::string unit = "";
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
      TestParameters p;
      
      p.size = 1;
      capture_result(p);
      
      p.size = 10;
      capture_result(p);
      
      p.size = 100;
      capture_result(p);
      
      p.size = 100;
      capture_result(p);
      
      p.size = 1000;
      capture_result(p);
      
      p.size = 1000;
      capture_result(p);
    }
  }
  
  void capture_result(const TestParameters &p)
  {
    Result result;
    result.parameters = p;
    m_current_result = &result;
  
    try
    {
      callback_impl();
    }
    catch(...)
    {
      m_current_result = nullptr;
      throw;
    }
  
    m_results.push_back(result);
  
  
    double value = result.parameters.size;
    double time = result.time.count();
    time /= 1e9;
  
    auto rate = value / time;
  
    std::cout << "Captured result " << p << "\n" <<
      "  with time " << result.time.count() << "ns\n" <<
      "  rate " << rate << " " << m_setup.unit << "/s" << std::endl;;
  }
  
  virtual void callback_impl() = 0;
  
  template <typename T> void run(const T &t)
  {
    auto begin = std::chrono::high_resolution_clock::now();
    t();
    auto end = std::chrono::high_resolution_clock::now();
  
    m_current_result->time = end - begin;
  }
  
  const TestParameters &parameters()
  {
    return m_current_result->parameters;
  }
  
  Setup &setup()
  {
    return m_setup;
  }
  
private:
  std::string m_name;
  Type m_type;
  Setup m_setup;
  
  struct Result
  {
    TestParameters parameters;
    std::chrono::nanoseconds time;
  };
  std::vector<Result> m_results;
  Result *m_current_result;
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
