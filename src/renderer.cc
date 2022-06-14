#include "renderer.h"

namespace sft {

Renderer::Renderer() {
  render_surface_size_ = {800, 800};
  window_size_ = render_surface_size_;
  window_size_.x *= 2.0;

  rasterizer_ = std::make_unique<Rasterizer>(render_surface_size_);

  if (!rasterizer_ || !rasterizer_->GetPixels()) {
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

  const auto size = rasterizer_->GetSize();

  SDLTextureNoCopyCaster color_attachment(renderer_,                       //
                                          rasterizer_->GetPixels(),        //
                                          size.x,                          //
                                          size.y,                          //
                                          rasterizer_->GetBytesPerPixel()  //

  );
  SDLTextureNoCopyCaster depth_attachment(
      renderer_,                            //
      rasterizer_->GetDepthPixels(),        //
      size.x,                               //
      size.y,                               //
      rasterizer_->GetDepthBytesPerPixel()  //

  );
  SDL_Rect dest = {};
  dest.x = 0;
  dest.y = 0;
  dest.w = size.x;
  dest.h = size.y;
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
  rasterizer_->SetDepthTestsEnabled(false);
  rasterizer_->Clear(kColorGreen);
  rasterizer_->SetDepthTestsEnabled(true);

  rasterizer_->DrawTriangle({render_surface_size_, 0},             //
                            {render_surface_size_, 0},             //
                            {render_surface_size_.x / 2.0, 0, 1},  //
                            kColorRed);
  rasterizer_->DrawTriangle(
      {0, 0, 0},                                                  //
      {render_surface_size_.x, 0, 0},                             //
      {render_surface_size_.x / 2.0, render_surface_size_.y, 1},  //
      kColorGreen);
  model_->RenderTo(*rasterizer_);
  rasterizer_->DrawLine({0, 0, 0},
                        {render_surface_size_.x, render_surface_size_.y, 1.0},
                        kColorBlue);
  return true;
}

}  // namespace sft
