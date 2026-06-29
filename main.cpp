#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

// Create framebuffer
std::vector<uint32_t> framebuffer(WIDTH *HEIGHT);

void place_pixel(int x, int y, uint32_t color) {
  if (x >= 0 && y >= 0 && x < WIDTH && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = color; // Map color to grid spot
  }
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
    place_pixel(WIDTH / 2, HEIGHT / 2, 0xFFFFFFFF);

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