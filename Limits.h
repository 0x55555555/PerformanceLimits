#pragma once

#include <chrono>
#include <iostream>
#include <vector>

namespace limits
{

struct TestParameters
{
  std::size_t count = 0;
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
    std::size_t maximum_count = 1000;
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

  void prevent_optimisation(const void *, std::size_t i);
  template <typename T> void prevent_optimisation(const T &t)
  {
    prevent_optimisation(&t, sizeof(t));
  }
  template <typename T> void prevent_optimisation(const std::vector<T> &t)
  {
    prevent_optimisation(t.data(), sizeof(t) * t.size());
  }
  
  const std::string &name() const { return m_name; }

  void callback()
  {
    TestParameters p;

    const std::chrono::milliseconds minimum(100);

    p.count = 1;
    do {
      for (std::size_t i = 0; i < 10; ++i) {
        capture_result(p);
      }
      
      p.count *= 10;
    } while(m_results.back().time < minimum && p.count <= m_setup.maximum_count);
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
  
  const Setup &setup() const
  {
    return m_setup;
  }
  
  const Result &final_result() { return m_results.back(); }
  const std::vector<Result> &results() { return m_results; }

  template <typename T, typename U>
  static double convert(const U &time)
  {
    double from = double(U::period::num) / double(U::period::den);
    double to = double(T::period::num) / double(T::period::den);
    return double(time.count()) * (from / to);
  }
  
  static double format_time(const std::chrono::nanoseconds &time, std::string &time_unit)
  {
    if (time > std::chrono::seconds(1)) {
      time_unit = "s";
      return convert<std::chrono::seconds>(time);
    }
  
    if (time > std::chrono::milliseconds(1)) {
      time_unit = "ms";
      return convert<std::chrono::milliseconds>(time);
    }
  
    if (time > std::chrono::microseconds(1)) {
      time_unit = "us";
      return convert<std::chrono::microseconds>(time);
    }
  
    time_unit = "ns";
    return time.count();
  }
  
  std::string rate(const Result &r) const
  {
    double value = r.parameters.count;
    std::string time_unit;
    double time = format_time(r.time, time_unit);
  
    auto rate = value / time;
  
    return std::to_string(rate) + " " + setup().unit + "/" + time_unit;
  }

  
  void dump(const Result &r)
  {
    std::string time_unit;
    double time = format_time(r.time, time_unit);
    std::cout << "  Captured result " << r.parameters << " with time " << time << time_unit <<
      " rate " << rate(r) << std::endl;
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
      std::cout << "  " << t->name() << ":\t";
      std::cout.flush();
    
      try
      {
        t->callback();
      }
      catch(...)
      {
        std::cerr << "    ... failed in test" << std::endl;
        all_success = false;
        continue;
      }
    
      TestBase::Result aggregated{};
      for (auto &r : t->results())
        {
        aggregated.time += r.time;
        aggregated.parameters.count += r.parameters.count;
      }
      aggregated.time = aggregated.time / t->results().size();
    
      std::cout << " total count " << aggregated.parameters.count << " over " << t->results().size() << " iterations, at rate " << t->rate(aggregated) << std::endl;
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
