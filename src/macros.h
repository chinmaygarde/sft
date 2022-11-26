/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <iostream>

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

#define SFT_ALWAYS_INLINE inline __attribute__((always_inline))
