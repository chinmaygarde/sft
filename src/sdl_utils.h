#pragma once

#include <SDL.h>

namespace sft {

struct SDLTextureNoCopyCaster {
  SDLTextureNoCopyCaster(SDL_Renderer* renderer,
                         void* pixels,
                         int width,
                         int height,
                         int bytes_per_pixel);

  ~SDLTextureNoCopyCaster();

  operator bool() const { return static_cast<SDL_Texture*>(*this) != NULL; }

  operator SDL_Texture*() const { return texture_; }

 private:
  SDL_Surface* surface_ = nullptr;
  SDL_Texture* texture_ = nullptr;

  SDLTextureNoCopyCaster(const SDLTextureNoCopyCaster&) = delete;
  SDLTextureNoCopyCaster& operator=(const SDLTextureNoCopyCaster&) = delete;
};

}  // namespace sft
