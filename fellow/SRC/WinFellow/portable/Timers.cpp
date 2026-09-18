/* Portable timer backend for non-Windows FellowNG runtimes. */
#include <atomic>
#include <chrono>
#include <cstdint>
#include <list>
#include <mutex>
#include <thread>

#include "Timers.h"

namespace {
std::atomic<bool> timer_running{false};
std::atomic<uint32_t> timer_ticks{0};
std::thread timer_thread;
std::mutex callback_mutex;
std::list<timerCallbackFunction> timer_callbacks;

void timerLoop()
{
  using namespace std::chrono;
  auto next = steady_clock::now();
  while (timer_running.load(std::memory_order_acquire))
  {
    next += milliseconds(1);
    std::this_thread::sleep_until(next);
    if (!timer_running.load(std::memory_order_acquire)) break;

    const uint32_t ticks = timer_ticks.fetch_add(1, std::memory_order_relaxed) + 1;
    std::lock_guard<std::mutex> lock(callback_mutex);
    for (timerCallbackFunction callback : timer_callbacks)
      if (callback != nullptr) callback(ticks);
  }
}
}

uint32_t timerGetTimeMs()
{
  using namespace std::chrono;
  return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

void timerAddCallback(timerCallbackFunction callback)
{
  std::lock_guard<std::mutex> lock(callback_mutex);
  timer_callbacks.push_back(callback);
}

void timerEmulationStart()
{
  if (timer_running.exchange(true, std::memory_order_acq_rel)) return;
  timer_ticks.store(0, std::memory_order_relaxed);
  timer_thread = std::thread(timerLoop);
}

void timerEmulationStop()
{
  if (timer_running.exchange(false, std::memory_order_acq_rel) && timer_thread.joinable())
    timer_thread.join();

  std::lock_guard<std::mutex> lock(callback_mutex);
  timer_callbacks.clear();
}

void timerStartup()
{
  timer_running.store(false, std::memory_order_relaxed);
  timer_ticks.store(0, std::memory_order_relaxed);
  std::lock_guard<std::mutex> lock(callback_mutex);
  timer_callbacks.clear();
}

void timerShutdown()
{
  timerEmulationStop();
}
