#include <SDL.h>
#include <iostream>
#include <memory>

struct Color {
  const uint32_t color = 0u;
  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
      : color(a << 24 | r << 16 | g << 8 | b << 0) {}
  constexpr operator uint32_t() const { return color; }
};

constexpr Color kColorRed = {255, 0, 0, 255};
constexpr Color kColorGreen = {0, 255, 0, 255};
constexpr Color kColorBlue = {0, 0, 255, 255};
constexpr Color kColorWhite = {255, 255, 255, 255};
constexpr Color kColorBlack = {0, 0, 0, 255};

struct Point {
  int32_t x = 0;
  int32_t y = 0;
};

class Image {
 public:
  Image(size_t p_width, size_t p_height)
      : allocation_(std::calloc(p_width * p_height * 4, sizeof(uint8_t))),
        width(p_width),
        height(p_height) {}

  ~Image() = default;

  void* GetPixels() const { return allocation_; }

  size_t GetWidth() const { return width; }

  size_t GetHeight() const { return height; }

  size_t GetBytesPerPixel() const { return sizeof(uint32_t); }

  bool Set(int32_t x, int32_t y, Color color) {
    if (!allocation_ || x < 0 || y < 0 || x >= width || y >= height) {
      return false;
    }

    auto ptr = reinterpret_cast<uint32_t*>(allocation_) + ((width * y) + x);
    *ptr = color;
    return true;
  }

  void Clear(Color color) {
    for (size_t j = 0; j < height; j++) {
      for (size_t i = 0; i < width; i++) {
        Set(i, j, color);
      }
    }
  }

  void DrawLine(Point p1, Point p2, Color color) {
    for (double i = 0.0; i <= 1.0; i += 0.01) {
      auto x = p1.x + ((p2.x - p1.x) * i);
      auto y = p1.y + ((p2.y - p1.y) * i);
      Set(x, y, color);
    }
  }

 private:
  void* allocation_ = nullptr;
  const size_t width;
  const size_t height;

  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;
};

class Renderer {
 public:
  Renderer() : image_(16, 16) {
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
    image_.DrawLine({1, 1}, {15, 15}, kColorBlack);
    return true;
  }

 private:
  Image image_;
  SDL_Surface* surface_ = nullptr;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool is_valid_ = false;

  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
};

int main(int argc, char const* argv[]) {
  if (::SDL_Init(SDL_INIT_VIDEO) != 0) {
    return EXIT_FAILURE;
  }

  bool is_running = true;

  Renderer renderer;
  while (is_running) {
    is_running = renderer.Render();
    ::SDL_Event event;
    if (::SDL_PollEvent(&event) == 1) {
      switch (event.type) {
        case SDL_QUIT:
          is_running = false;
          break;
      }
    }
  }

  ::SDL_Quit();

  return 0;
}
