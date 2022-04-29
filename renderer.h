#pragma once

#include <SDL.h>
#include <memory>
#include "geom.h"
#include "image.h"
#include "model.h"

namespace sft {

class Renderer {
 public:
  Renderer() : image_(800, 800) {
    if (!image_.GetPixels()) {
      return;
    }
    surface_ = ::SDL_CreateRGBSurfaceFrom(
        image_.GetPixels(),                             //
        image_.GetWidth(),                              //
        image_.GetHeight(),                             //
        image_.GetBytesPerPixel() * 8u,                 //
        image_.GetWidth() * image_.GetBytesPerPixel(),  //
        0, 0, 0, 0);
    if (!surface_) {
      return;
    }

    window_ = ::SDL_CreateWindow("SFT", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 800, 800, 0);
    if (!window_) {
      return;
    }

    renderer_ = ::SDL_CreateRenderer(window_, -1, 0);
    if (!renderer_) {
      return;
    }

    model_ = std::make_unique<Model>("../assets/teapot/teapot.obj");
    if (!model_->IsValid()) {
      return;
    }

    is_valid_ = true;
  }

  ~Renderer() {
    if (surface_) {
      ::SDL_FreeSurface(surface_);
    }
    if (window_) {
      ::SDL_DestroyWindow(window_);
    }
    if (renderer_) {
      ::SDL_DestroyRenderer(renderer_);
    }
  }

  bool Render() {
    if (!is_valid_) {
      return false;
    }

    if (!Update()) {
      return false;
    }

    auto texture_deleter = [](SDL_Texture* tex) {
      if (tex) {
        ::SDL_DestroyTexture(tex);
      }
    };
    std::unique_ptr<SDL_Texture, decltype(texture_deleter)> texture(
        ::SDL_CreateTextureFromSurface(renderer_, surface_), texture_deleter);
    if (!texture) {
      return false;
    }

    if (::SDL_RenderCopy(renderer_, texture.get(), nullptr, nullptr) != 0) {
      return false;
    }

    ::SDL_RenderPresent(renderer_);

    return true;
  }

  bool Update() {
    image_.Clear(kColorWhite);

    image_.DrawLine({10, 10}, {500, 500}, kColorRed);
    image_.DrawLine({10, 500}, {500, 10}, kColorGreen);

    image_.DrawTriangle({200, 200}, {300, 100}, {300, 300}, kColorBlack);

    // model_->RenderTo(image_);
    return true;
  }

 private:
  Image image_;
  SDL_Surface* surface_ = nullptr;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<Model> model_;
  bool is_valid_ = false;

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
};

}  // namespace sft
