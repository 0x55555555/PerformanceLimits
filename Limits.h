#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace limits
{

struct TestParameters
{
  std::size_t count;
};

inline std::ostream &operator<<(std::ostream &str, const TestParameters &p)
{
  return str << "{ " << p.count << " }";
}

class TestBase
{
public:
  enum Type
  {
    Sized,
    Count
  };

  struct Setup
  {
    std::string unit = "";
  };

  struct Result
  {
    TestParameters parameters;
    std::chrono::nanoseconds time;
  };

  TestBase(const char *name, Type t)
  : m_name(name)
  , m_type(t)
  {
  }

  const std::string &name() const { return m_name; }

  void callback()
  {
    TestParameters p;

    const std::chrono::milliseconds minimum(200);

    p.count = 1;
    do {
      capture_result(p);
      p.count *= 10;
    } while(m_results.back().time < minimum);
  }

  void capture_result(const TestParameters &p)
  {
    Result result;
    result.parameters = p;
    m_current_result = &result;

    try
    {
      if (m_type == Sized) {
        callback_impl();
      }
      else {

        callback_impl();
      }
    }
    catch(...)
    {
      m_current_result = nullptr;
      throw;
    }

    m_results.push_back(result);
    dump(result);
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

  const Result &final_result() { return m_results.back(); }

  void dump(const Result &r)
  {
    double value = r.parameters.count;
    double time = r.time.count();
    auto time_unit = "ns";
    if (time > 1000000000) {
      time_unit = "s";
      time /= 1e9;
    }
    if (time > 1000000) {
      time_unit = "us";
      time /= 1e6;
    }
    if (time > 1000) {
      time_unit = "ms";
      time /= 1e3;
    }

    auto rate = value / time;

    std::cout << "  Captured result with time " << time << time_unit <<
      " rate " << rate << " " << setup().unit << "/" << time_unit << std::endl;
  }

private:
  std::string m_name;
  Type m_type;
  Setup m_setup;

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

      t->dump(t->final_result());
    }

    std::cout << "... all tests complete" << std::endl;
    return all_success ? 0 : 1;
  }

  static std::vector<TestBase *> m_tests;
};

}

#define LIMITS_JOIN2(a, b) a ## b
#define LIMITS_JOIN(a, b) LIMITS_JOIN2(a, b)
#define LIMITS_TEST_NAME LIMITS_JOIN(Test, __LINE__)

#define LIMITS_TEST(name, type) \
  struct LIMITS_TEST_NAME : \
    public limits::TestBase { \
      LIMITS_TEST_NAME() : TestBase(#name, limits::TestBase::type) \
        { limits::Tests::install(this); } \
      void callback_impl() override; }; \
  LIMITS_TEST_NAME LIMITS_JOIN(g_, LIMITS_TEST_NAME); \
  void LIMITS_TEST_NAME::callback_impl()

#define LIMITS_APP() int main(int argc, char **argv) { limits::Tests tests(argc, argv); return tests.run(); }
