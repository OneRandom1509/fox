#include "clock.hpp"
#include <chrono>

auto Clock::arity() -> size_t { return 0; }

auto Clock::call(Interpreter interpreter, std::vector<literal_t> arguments)
  -> literal_t
{
  return (double)std::chrono::system_clock::now().time_since_epoch().count()
         / 1000000000.0; // nanoseconds to seconds
}

auto Clock::toString() -> std::string { return "<native fn>"; }
