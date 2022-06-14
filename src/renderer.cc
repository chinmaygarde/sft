#include "renderer.h"

namespace sft {

Renderer::Renderer() {
  render_surface_size_ = {800, 800};
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

Renderer::~Renderer() {
  if (window_) {
    ::SDL_DestroyWindow(window_);
  }
  if (renderer_) {
    ::SDL_DestroyRenderer(renderer_);
  }
}

bool Renderer::Render() {
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

bool Renderer::Update() {
  image_->SetDepthTestsEnabled(false);
  image_->Clear(kColorGreen);
  image_->SetDepthTestsEnabled(true);

  image_->DrawTriangle({render_surface_size_, 0},             //
                       {render_surface_size_, 0},             //
                       {render_surface_size_.x / 2.0, 0, 1},  //
                       kColorRed);
  image_->DrawTriangle(
      {0, 0, 0},                                                  //
      {render_surface_size_.x, 0, 0},                             //
      {render_surface_size_.x / 2.0, render_surface_size_.y, 1},  //
      kColorGreen);
  model_->RenderTo(*image_);
  image_->DrawLine({0, 0, 0},
                   {render_surface_size_.x, render_surface_size_.y, 1.0},
                   kColorBlue);
  return true;
}

}  // namespace sft
