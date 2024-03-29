/*
 *  This source file is part of the SFT project.
 *  Licensed under the MIT License. See LICENSE file for details.
 */

#include "sdl_utils.h"

#include "geometry.h"

namespace sft {

SDLTextureNoCopyCaster::SDLTextureNoCopyCaster(SDL_Renderer* renderer,
                                               const void* pixels,
                                               int width,
                                               int height,
                                               int bytes_per_pixel) {
  if (pixels == nullptr) {
    return;
  }
  auto surface =
      ::SDL_CreateRGBSurfaceFrom(const_cast<void*>(pixels),       //
                                 width,                           //
                                 height,                          //
                                 bytes_per_pixel * 8,             //
                                 width * bytes_per_pixel,         //
                                 kColorRed.WithAlpha(0).color,    // r mask
                                 kColorGreen.WithAlpha(0).color,  // g mask
                                 kColorBlue.WithAlpha(0).color,   // b mask
                                 kColorTransparentBlack.color     // a mask
      );
  if (surface == NULL) {
    return;
  }
  surface_ = surface;
  auto texture = ::SDL_CreateTextureFromSurface(renderer, surface_);
  if (texture == NULL) {
    return;
  }
  texture_ = texture;
}

SDLTextureNoCopyCaster::~SDLTextureNoCopyCaster() {
  if (texture_) {
    ::SDL_DestroyTexture(texture_);
  }

  if (surface_) {
    ::SDL_FreeSurface(surface_);
  }
}

SDLTextureNoCopyCaster::operator bool() const {
  return static_cast<SDL_Texture*>(*this) != NULL;
}

SDLTextureNoCopyCaster::operator SDL_Texture*() const {
  return texture_;
}

}  // namespace sft
