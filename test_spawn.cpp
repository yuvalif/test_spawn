#ifdef USE_BOOST_SPAWN
#include <boost/asio/spawn.hpp>
#else
#include "spawn/spawn.hpp"
#endif
#include <boost/asio/io_context.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/context/protected_fixedsize_stack.hpp>
#include <boost/coroutine/attributes.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <pthread.h>

namespace ba = boost::asio;

using Clock = std::chrono::steady_clock;
using Timer = ba::basic_waitable_timer<Clock, ba::wait_traits<Clock>>;

#ifdef TEST_CRASH
constexpr auto stack_size_kb = 128U;
#else
constexpr auto stack_size_kb = 256U;
#endif

auto make_stack_allocator() {
#ifdef USE_BOOST_SPAWN
   return boost::coroutines::attributes(stack_size_kb*1024);
#else
  return boost::context::protected_fixedsize_stack{stack_size_kb*1024};
#endif
}

#ifdef USE_BOOST_SPAWN
namespace spawn_ns = ba;
#else
namespace spawn_ns = spawn;
#endif

class Counter {
  static constexpr auto sleep_time = 1U;
  std::thread worker;
  ba::io_context io_context;
  ba::executor_work_guard<ba::io_context::executor_type> work_guard;

public:
  Counter() : work_guard(ba::make_work_guard(io_context)) 
  {
    spawn_ns::spawn(io_context, [this](spawn_ns::yield_context yield) {
      char buff[1024*128] = {'0'};
      auto i = 0;
      Timer timer(io_context);
      while (true) {	
        timer.expires_from_now(std::chrono::seconds(sleep_time));
        timer.async_wait(yield);
        std::cout << "inside the CR. i = " << i << std::endl;
        ++i;
      }
    }, make_stack_allocator());

    worker = std::thread([this](){io_context.run();});
  }

  ~Counter() {
    work_guard.reset();
    io_context.stop();
    worker.join();
  }
};

int main() {
  Counter c;
  std::this_thread::sleep_for(std::chrono::seconds(10));

  return 0;
}

