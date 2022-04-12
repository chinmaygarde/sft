#include <iostream>

struct Color {
  uint32_t color = 0u;
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    color = r << 24 | g << 16 | b << 8 | a;
  }
  operator uint32_t() const { return color; }
};

class Image {
 public:
  Image(size_t p_width, size_t p_height)
      : allocation_(std::malloc(p_width * p_height * 4)),
        width(p_width),
        height(p_height) {}

  ~Image() = default;

  bool Set(size_t x, size_t y, Color color) {
    if (!allocation_ || x >= width || y >= height) {
      return false;
    }

    auto ptr = reinterpret_cast<uint32_t*>(allocation_) + (x * y);
    *ptr = color;
    return true;
  }

 private:
  void* allocation_ = nullptr;
  const size_t width;
  const size_t height;
};

int main(int argc, char const* argv[]) {
  auto image = Image{256, 256};
  return 0;
}
