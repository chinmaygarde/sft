#include "sdl_utils.h"

namespace sft {

SDLTextureNoCopyCaster::SDLTextureNoCopyCaster(SDL_Renderer* renderer,
                                               void* pixels,
                                               int width,
                                               int height,
                                               int bytes_per_pixel) {
  auto surface = ::SDL_CreateRGBSurfaceFrom(pixels,                   //
                                            width,                    //
                                            height,                   //
                                            bytes_per_pixel * 8,      //
                                            width * bytes_per_pixel,  //
                                            0, 0, 0, 0                // masks
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

}  // namespace sft
