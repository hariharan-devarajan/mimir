//
// Created by hariharan on 3/8/22.
//

#ifndef MIMIR_DEBUG_H
#define MIMIR_DEBUG_H

#include <chrono>

#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
#include <mpi.h>

#include <iostream>
#include <sstream>
#include <tuple>
#endif
namespace mimir {
class Timer {
 public:
  Timer() : elapsed_time(0) {}
  void resumeTime() { t1 = std::chrono::high_resolution_clock::now(); }
  double pauseTime() {
    auto t2 = std::chrono::high_resolution_clock::now();
    elapsed_time += std::chrono::duration<double>(t2 - t1).count();
    return elapsed_time;
  }
  double getElapsedTime() { return elapsed_time; }

 private:
  std::chrono::high_resolution_clock::time_point t1;
  double elapsed_time;
};
class AutoTrace {
#if defined(MIMIR_TIMER)
  Timer timer;
#endif
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
  static int rank, item;
  std::string m_line;
#endif
 public:
  template <typename... Args>
  AutoTrace(
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
      std::string string,
#endif
      Args... args)
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
      : m_line(string)
#endif
  {
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
    char thread_name[256];
    pthread_getname_np(pthread_self(), thread_name, 256);
    std::stringstream stream;

    if (rank == -1) MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    stream << "\033[31m";
    stream << ++item << ";" << thread_name << ";" << rank << ";" << m_line
           << ";";
#endif
#if defined(MIMIR_TIMER)
    stream << ";;";
#endif
#ifdef MIMIR_TRACE
    auto args_obj = std::make_tuple(args...);
    const ulong args_size = std::tuple_size<decltype(args_obj)>::value;
    stream << "args(";

    if (args_size == 0)
      stream << "Void";
    else {
      std::apply([&stream](auto&&... args) { ((stream << args << ", "), ...); },
                 args_obj);
    }
    stream << ");";
#endif
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
    stream << "start" << std::endl;
    stream << "\033[00m";
    std::cout << stream.str();
#endif
#ifdef MIMIR_TIMER
    timer.resumeTime();
#endif
  }

  ~AutoTrace() {
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
    std::stringstream stream;
    char thread_name[256];
    pthread_getname_np(pthread_self(), thread_name, 256);
    stream << "\033[31m";
    stream << item-- << ";" << std::string(thread_name) << ";" << rank << ";"
           << m_line << ";";
#endif
#if defined(MIMIR_TRACE)
    stream << ";";
#endif
#ifdef MIMIR_TIMER
    double end_time = timer.pauseTime();
    stream << end_time << ";msecs;";
#endif
#if defined(MIMIR_TRACE) || defined(MIMIR_TIMER)
    stream << "finish" << std::endl;
    stream << "\033[00m";
    std::cout << stream.str();
#endif
  }
};

}  // namespace mimir
#endif  // MIMIR_DEBUG_H
