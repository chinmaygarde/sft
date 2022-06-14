#pragma once

#define SFT_ASSERT(x)                                       \
  {                                                         \
    if (!(x)) [[unlikely]] {                                \
      std::cout << "Assertion " #x " failed." << std::endl; \
      std::abort();                                         \
    }                                                       \
  }
