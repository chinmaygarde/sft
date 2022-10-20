#pragma once

#include <iostream>

#include "tracing.h"

#define SFT_ASSERT(x)                                            \
  {                                                              \
    if (!(x)) {                                                  \
      std::cout << "SFT: Assertion " #x " failed." << std::endl; \
      std::abort();                                              \
    }                                                            \
  }

#define SFT_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete

#define SFT_DISALLOW_ASSIGN(TypeName) \
  TypeName& operator=(const TypeName&) = delete

#define SFT_DISALLOW_MOVE(TypeName) \
  TypeName(TypeName&&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define SFT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;          \
  TypeName& operator=(const TypeName&) = delete

#define SFT_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName) \
  TypeName(const TypeName&) = delete;               \
  TypeName(TypeName&&) = delete;                    \
  TypeName& operator=(const TypeName&) = delete;    \
  TypeName& operator=(TypeName&&) = delete

#define SFT_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                               \
  SFT_DISALLOW_COPY_ASSIGN_AND_MOVE(TypeName)
