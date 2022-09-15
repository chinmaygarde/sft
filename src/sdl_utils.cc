#include "sdl_utils.h"

namespace sft {

SDLTextureNoCopyCaster::SDLTextureNoCopyCaster(SDL_Renderer* renderer,
                                               void* pixels,
                                               int width,
                                               int height,
                                               int bytes_per_pixel) {
  if (pixels == nullptr) {
    return;
  }
  auto surface = ::SDL_CreateRGBSurfaceFrom(pixels,                   //
                                            width,                    //
                                            height,                   //
                                            bytes_per_pixel * 8,      //
                                            width * bytes_per_pixel,  //
                                            0,                        // r mask
                                            0,                        // g mask
                                            0,                        // b mask
                                            0                         // a mask
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
