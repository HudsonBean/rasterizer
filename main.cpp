#include "geometry.h"
#include "mat.h"
#include "vec.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

// Create framebuffer
std::vector<uint32_t> framebuffer(WIDTH *HEIGHT);

void draw_pixel(int x, int y, uint32_t color) {
  if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = color;
  }
}

void draw_pixel(Vec2 a, uint32_t color) {
  draw_pixel(static_cast<int>(a.x), static_cast<int>(a.y), color);
}

// Bresenham's line drawing algorithm
void draw_line_horizontal(int x0, int y0, int x1, int y1, uint32_t color) {
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int dx = x1 - x0;
  int dy = y1 - y0;

  int dir = (dy < 0) ? -1 : 1;
  dy *= dir;

  if (dx != 0) {
    int y = y0;
    int drift = 2 * dy - dx;

    for (int i = 0; i < (dx + 1); i++) {
      draw_pixel(x0 + i, y, color);

      if (drift >= 0) {
        y += dir;
        drift = drift - 2 * dx;
      }

      drift = drift + 2 * dy;
    }
  }
}

void draw_line_vertical(int x0, int y0, int x1, int y1, uint32_t color) {
  if (y0 > y1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int dx = x1 - x0;
  int dy = y1 - y0;

  int dir = (dx < 0) ? -1 : 1;
  dx *= dir;

  if (dy != 0) {
    int x = x0;
    int drift = 2 * dx - dy;

    for (int i = 0; i < (dy + 1); i++) {
      draw_pixel(x, y0 + i, color);

      if (drift >= 0) {
        x += dir;
        drift = drift - 2 * dy;
      }

      drift = drift + 2 * dx;
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
  if (std::abs(x1 - x0) > std::abs(y1 - y0)) {
    draw_line_horizontal(x0, y0, x1, y1, color);
  } else {
    draw_line_vertical(x0, y0, x1, y1, color);
  }
  Vec2 a{static_cast<float>(x0), static_cast<float>(y0)};
  Vec2 b{static_cast<float>(x1), static_cast<float>(y1)};
}

void draw_line(Vec2 a, Vec2 b, uint32_t color) {
  draw_line(static_cast<int>(a.x), static_cast<int>(a.y), static_cast<int>(b.x),
            static_cast<int>(b.y), color);
}

void draw_triangle(Vec2 a, Vec2 b, Vec2 c, uint32_t color) {
  draw_line(a, b, color);
  draw_line(b, c, color);
  draw_line(c, a, color);
}

void fill_triangle(Vec2 a, Vec2 b, Vec2 c, uint32_t color) {
  const int min_x = static_cast<int>(std::floor(std::min({a.x, b.x, c.x})));
  const int max_x = static_cast<int>(std::ceil(std::max({a.x, b.x, c.x})));
  const int min_y = static_cast<int>(std::floor(std::min({a.y, b.y, c.y})));
  const int max_y = static_cast<int>(std::ceil(std::max({a.y, b.y, c.y})));

  for (int y = min_y; y <= max_y; y++) {
    for (int x = min_x; x <= max_x; x++) {
      Vec2 p{static_cast<float>(x), static_cast<float>(y)};

      const float triangle_area = orient2D(a, b, c);
      const float w0 = orient2D(a, b, p);
      const float w1 = orient2D(b, c, p);
      const float w2 = orient2D(c, a, p);

      // Barycentric weights
      const float alpha = w1 / triangle_area;
      const float beta = w2 / triangle_area;
      const float gamma = w0 / triangle_area;

      const bool has_neg = (w0 < 0) || (w1 < 0) || (w2 < 0);
      const bool has_pos = (w0 > 0) || (w1 > 0) || (w2 > 0);
      if (!(has_neg && has_pos)) {
        draw_pixel(x, y, color);
      }
    }
  }
}

float to_rad(int deg) { return deg * (std::numbers::pi / 180); }

Mat4 perspective(float fov_y, float aspect, float near, float far) {}

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Rasterizer", WIDTH, HEIGHT, 0);
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (!texture) {
    SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  bool running = true;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
    }

    for (auto &pixel : framebuffer) {
      pixel = 0xFF000000;
    }

    // ––––––––––––––––––––––––––––––––––––––––––––––––––

    // Back triangle (far)
    Vec3 a0{200, 120, 0.0f};
    Vec3 b0{520, 220, 0.0f};
    Vec3 c0{280, 420, 0.0f};
    fill_triangle(Vec2{a0.x, a0.y}, Vec2{b0.x, b0.y}, Vec2{c0.x, c0.y},
                  0xFFFF0000);

    // Middle triangle
    Vec3 a1{260, 160, 0.5f};
    Vec3 b1{480, 260, 0.5f};
    Vec3 c1{340, 380, 0.5f};
    fill_triangle(Vec2{a1.x, a1.y}, Vec2{b1.x, b1.y}, Vec2{c1.x, c1.y},
                  0xFF00FF00);

    // Front triangle (near)
    Vec3 a2{320, 200, 1.0f};
    Vec3 b2{440, 280, 1.0f};
    Vec3 c2{380, 340, 1.0f};
    fill_triangle(Vec2{a2.x, a2.y}, Vec2{b2.x, b2.y}, Vec2{c2.x, c2.y},
                  0xFF0000FF);

    // ––––––––––––––Perspective Projection––––––––––––––
    // Maybe move these somewhere else later on
    Vec3 camera_pos{0, 0, 0};
    float fov = to_rad(60);
    float aspect = 1920.0f / 1080.0f;

    // Camera inverse transform
    Mat4 view = Mat4::translate({-camera_pos.x, -camera_pos.y, -camera_pos.z});
    Mat4 proj = perspective(fov, aspect, 0.5f, 100.0f);

    // ––––––––––––––––––––––––––––––––––––––––––––––––––

    SDL_UpdateTexture(texture, nullptr, framebuffer.data(),
                      WIDTH * sizeof(uint32_t));
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
