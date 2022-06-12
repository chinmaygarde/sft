#pragma once

#include <SDL.h>
#include <memory>
#include "geom.h"
#include "image.h"
#include "model.h"
#include "sdl_utils.h"

namespace sft {

class Renderer {
 public:
  Renderer() {
    render_surface_size_ = {400, 400};
    window_size_ = render_surface_size_;
    window_size_.x *= 2.0;

    image_ = std::make_unique<Image>(render_surface_size_);

    if (!image_ || !image_->GetPixels()) {
      return;
    }

#ifndef NDEBUG
#define SFT_DEBUG_TITLE "Debug Build"
#else
#define SFT_DEBUG_TITLE "Release Build"
#endif

    window_ = ::SDL_CreateWindow("SFT Sandbox (" SFT_DEBUG_TITLE
                                 ") (Press \"q\" or ESC to quit)",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 window_size_.x, window_size_.y, 0);
    if (!window_) {
      return;
    }

    renderer_ = ::SDL_CreateRenderer(window_, -1, 0);
    if (!renderer_) {
      return;
    }

    model_ = std::make_unique<Model>("assets/teapot/teapot.obj");
    if (!model_->IsValid()) {
      return;
    }

    is_valid_ = true;
  }

  ~Renderer() {
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

    SDLTextureNoCopyCaster color_attachment(renderer_,                  //
                                            image_->GetPixels(),        //
                                            image_->GetWidth(),         //
                                            image_->GetHeight(),        //
                                            image_->GetBytesPerPixel()  //

    );
    SDLTextureNoCopyCaster depth_attachment(renderer_,                       //
                                            image_->GetDepthPixels(),        //
                                            image_->GetWidth(),              //
                                            image_->GetHeight(),             //
                                            image_->GetDepthBytesPerPixel()  //

    );
    SDL_Rect dest = {};
    dest.x = 0;
    dest.y = 0;
    dest.w = image_->GetWidth();
    dest.h = image_->GetHeight();
    if (::SDL_RenderCopy(renderer_, color_attachment, nullptr, &dest) != 0) {
      return false;
    }
    dest.x += dest.w;
    if (::SDL_RenderCopy(renderer_, depth_attachment, nullptr, &dest) != 0) {
      return false;
    }
    ::SDL_RenderPresent(renderer_);
    return true;
  }

  bool Update() {
    image_->SetDepthTestsEnabled(false);
    image_->Clear(kColorWhite);
    image_->SetDepthTestsEnabled(true);
    model_->RenderTo(*image_);
    image_->DrawTriangle({0, 400, 0},    //
                         {400, 400, 0},  //
                         {200, 0, 1},    //
                         kColorRed);
    image_->DrawTriangle({0, 0, 0},      //
                         {400, 0, 0},    //
                         {200, 400, 1},  //
                         kColorGreen);
    image_->DrawLine({0, 0, 0},
                     {render_surface_size_.x, render_surface_size_.y, 1.0},
                     kColorBlue);
    return true;
  }

 private:
  std::unique_ptr<Image> image_;
  glm::ivec2 render_surface_size_;
  glm::ivec2 window_size_;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<Model> model_;
  bool is_valid_ = false;

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
};

}  // namespace sft
