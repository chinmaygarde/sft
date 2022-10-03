#pragma once

#include <SDL.h>
#include "macros.h"

namespace sft {

struct SDLTextureNoCopyCaster {
  SDLTextureNoCopyCaster(SDL_Renderer* renderer,
                         const void* pixels,
                         int width,
                         int height,
                         int bytes_per_pixel);

  ~SDLTextureNoCopyCaster();

  operator bool() const;

  operator SDL_Texture*() const;

 private:
  SDL_Surface* surface_ = nullptr;
  SDL_Texture* texture_ = nullptr;

  SFT_DISALLOW_COPY_AND_ASSIGN(SDLTextureNoCopyCaster);
};

}  // namespace sft
