#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

// Create framebuffer
std::vector<uint32_t> framebuffer(WIDTH *HEIGHT);

struct pixel {
  int x;
  int y;
};

void draw_pixel(int x, int y, uint32_t color) {
  if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = color; // Map color to grid spot
  }
}
void draw_pixel(pixel a, uint32_t color) {
  int x = a.x;
  int y = a.y;

  if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = color; // Map color to grid spot
  }
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

      // Make shift decision
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

      // Make shift decision
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
}
void draw_line(pixel a, pixel b, uint32_t color) {
  int x0 = a.x;
  int y0 = a.y;
  int x1 = b.x;
  int y1 = b.y;

  if (std::abs(x1 - x0) > std::abs(y1 - y0)) {
    draw_line_horizontal(x0, y0, x1, y1, color);
  } else {
    draw_line_vertical(x0, y0, x1, y1, color);
  }
}

// Draw triangle faces
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color) {
  draw_line(x0, y0, x1, y1, color);
  draw_line(x1, y1, x2, y2, color);
  draw_line(x2, y2, x0, y0, color);
}
void draw_triangle(pixel a, pixel b, pixel c, uint32_t color) {
  int x0 = a.x;
  int y0 = a.y;
  int x1 = b.x;
  int y1 = b.y;
  int x2 = c.x;
  int y2 = c.y;

  draw_line(a, b, color);
  draw_line(b, c, color);
  draw_line(c, a, color);
}

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow("Rasterizer", WIDTH, HEIGHT, 0);
  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create texture
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (!texture) {
    SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create event loop
  bool running = true;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
    }
    // Clear screen to black
    for (auto &pixel : framebuffer) {
      pixel = 0xFF000000;
    }

    // Draw pixel to center
    pixel center_pixel{WIDTH / 2, HEIGHT / 2};
    draw_pixel(center_pixel, 0xFFFFFFFF);

    // Draw line
    draw_line(100, 100, 500, 500, 0xFFFFFFFF);

    // Draw triangle
    draw_triangle(240, 150, 500, 240, 300, 400, 0xFFFFFFFF);

    // Update the texture
    SDL_UpdateTexture(texture, nullptr, framebuffer.data(),
                      WIDTH * sizeof(uint32_t));

    // Draw texture to screen
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    // Display
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}