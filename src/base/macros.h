/*
 * macros.h
 *
 *  Created on: Apr 1, 2014
 *      Author: gregerso
 *
 *  Convenience macros and macro-like functions/classes.
 */

#ifndef SIGNAL_CONTENT_BASE_MACROS_H_
#define SIGNAL_CONTENT_BASE_MACROS_H_

#include <cassert>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

#ifndef ENV_VERBOSITY_LEVEL
#define ENV_VERBOSITY_LEVEL 0
#endif

#ifndef ENV_DEBUG_ENABLED
#define ENV_DEBUG_ENABLED 0
#endif

#ifndef ENV_DEBUG_LEVEL
#define ENV_DEBUG_LEVEL 0
#endif

// For printing the current file, line number, and function name to a stream
#define CONTEXT_AS_STREAM_ARG __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__

// Exits if pointer is NULL, else returns pointer.
template<typename T> inline T* CHECK_NOTNULL(T* ptr) { assert(ptr != nullptr); return ptr; }

// Macros for stream-like logging and assertions.
#define LOG MacroLikeLogger()
#define CHECK(expr) MacroLikeCheckLogger(expr) << CONTEXT_AS_STREAM_ARG << " - CHECK FAILED - " << #expr << std::endl
#define CHECK_EQ(a, b) MacroLikeBinaryCheckLogger<decltype(a)>([] (const decltype(a)& arg1, const decltype(b)& arg2) { return arg1 == arg2; }) << CONTEXT_AS_STREAM_ARG << " - CHECK FAILED" << std::endl

#if ENV_DEBUG_ENABLED
#define DCHECK(expr) CHECK(expr)
#define DCHECK_EQ(a, b) CHECK_EQ(a, b)
#define DLOG(lev) MacroLikeDebugLogger(lev)
#else
#define DCHECK(expr) BlackHoleLogger()
#define DCHECK_EQ(a, b) BlockHoleLogger()
#define DLOG(lev) BlackHoleLogger()
#endif

// Consumes stream arguments, but emits nothing.
class BlackHoleLogger {
  BlackHoleLogger& operator << (const char* cstr) {
    return *this;
  }
  BlackHoleLogger& operator << (const std::string& str) {
    return *this;
  }
};

class MacroLikeLogger {
 public:
  MacroLikeLogger() {}
  MacroLikeLogger(bool enabled, const std::ostream& ostr)
      : enabled_(enabled), ostr_(ostr) {}

  MacroLikeLogger& operator << (const char* cstr) {
    if (enabled_) ostr_ << cstr;
    return *this;
  }
  MacroLikeLogger& operator << (const std::string& str) {
    if (enabled_) ostr_ << str;
    return *this;
  }
  virtual ~MacroLikeLogger() {
    if (enabled_) ostr_ << std::endl;
  }

 protected:
  bool enabled() {
    return enabled_;
  }
  void set_enabled(bool enabled) {
    enabled_ = enabled;
  }
  std::ostream& ostr() {
    return ostr_;
  }
  void set_stream(const std::ostream& ostr) {
    ostr_ = ostr;
  }

 private:
  bool enabled_{true};
  std::ostream ostr_{std::cout};
};

class MacroLikeDebugLogger : public MacroLikeLogger {
 public:
  MacroLikeDebugLogger(int lev)
      : MacroLikeLogger((lev >= ENV_DEBUG_LEVEL), std::cout) {};
};

class MacroLikeVerboseLogger : public MacroLikeLogger {
 public:
  MacroLikeVerboseLogger(int lev)
      : MacroLikeLogger((lev >= ENV_VERBOSITY_LEVEL), std::cout) {};
};

class MacroLikeCheckLogger : public MacroLikeLogger {
 public:
  MacroLikeCheckLogger(bool passed)
      : MacroLikeLogger(!passed, std::cerr) {}
  virtual ~MacroLikeCheckLogger() {
    if (enabled()) {
      exit(1);
    }
  }
};

template <typename T>
class MacroLikeBinaryCheckLogger : public MacroLikeLogger {
 public:
  MacroLikeBinaryCheckLogger(std::function<bool(const T&, const T&)> f,
                             const T& a, const T&b) : a_(a), b_(b) {
    bool passed = f(a, b);
    set_enabled(!passed);
  }
  virtual ~MacroLikeBinaryCheckLogger() {
    if (enabled()) {
      ostr() << std::endl << "ARGS: (" << a_ << ", " << b_ << ")";
      exit(1);
    }
  }
 private:
  const T a_;
  const T b_;
};

#endif /* SIGNAL_CONTENT_BASE_MACROS_H_ */
