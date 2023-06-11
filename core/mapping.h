/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <functional>
#include <memory>

#include "macros.h"

namespace sft {

class Mapping {
 public:
  static std::unique_ptr<Mapping> MakeWithCopy(const uint8_t* buffer,
                                               size_t size);

  Mapping(const uint8_t* buffer,
          size_t size,
          std::function<void(void)> on_done = nullptr);

  ~Mapping();

  const uint8_t* GetBuffer() const;

  size_t GetSize() const;

 private:
  const uint8_t* buffer_;
  size_t size_;
  std::function<void(void)> on_done_;

  SFT_DISALLOW_COPY_AND_ASSIGN(Mapping);
};

}  // namespace sft
