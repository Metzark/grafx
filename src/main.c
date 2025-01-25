#pragma region Include

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#pragma endregion Include

#pragma region Macros

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#pragma endregion Macros

#pragma region Declarations

struct GUI {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

struct GUI create_gui();

void init_sdl();

void draw_circle(SDL_Renderer *renderer, int centerX, int centerY, int radius);

#pragma endregion Declarations

int main(int argc, char *argv[]) {

    // Initialize SDL
    init_sdl();

    // Create GUI with window and renderer
    struct GUI gui = create_gui();

    int running = SDL_TRUE;
    SDL_Event event;

    int dragging = 0;
    int startX = 0, startY = 0;

    int center_x = WINDOW_WIDTH / 2 - 25;
    int center_y = WINDOW_HEIGHT / 2 - 25;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        dragging = 1;
                        startX = event.button.x - center_x;
                        startY = event.button.y - center_y;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (dragging) {
                        center_x = event.motion.x - startX;
                        center_y = event.motion.y - startY;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT && dragging) {
                        dragging = 0;
                    }
                    break;
                default:
                    break;
            }
        }

        // Clear the renderer with a color
        SDL_SetRenderDrawColor(gui.renderer, 0, 0, 0, 0);
        SDL_RenderClear(gui.renderer);

        // Set color
        SDL_SetRenderDrawColor(gui.renderer, 255, 255, 0, 255);

        // Draw circle
        draw_circle(gui.renderer, center_x, center_y, 50);

        // Present the renderer (show rendered content on screen)
        SDL_RenderPresent(gui.renderer);

        // Limit frame rate (approx. 60 FPS)
        SDL_Delay(16);
    }

    // Destroy the renderer
    SDL_DestroyRenderer(gui.renderer);

    // Destroy the window
    SDL_DestroyWindow(gui.window);

    // Quit SDL
    SDL_Quit();

    return 0;
}

// Initialize SDL Library
void init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }
}

// Create a GUI initialized with a window and renderer
struct GUI create_gui() {
    struct GUI new_gui;

    // Create an SDL window
    new_gui.window = SDL_CreateWindow(
        "Grafx",           
        SDL_WINDOWPOS_CENTERED,  
        SDL_WINDOWPOS_CENTERED,  
        800,                     
        600,                     
        SDL_WINDOW_SHOWN         
    );

    // If window is null, exit
    if (new_gui.window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Create an SDL renderer
    new_gui.renderer = SDL_CreateRenderer(
        new_gui.window,                   
        -1,                       
        SDL_RENDERER_ACCELERATED
    );

    // If renderer is null, exit
    if (new_gui.renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(new_gui.window);
        SDL_Quit();
        exit(1);
    }

    return new_gui;
}

// Draws a filled in circle with radius r, centered at (c_x, c_y)
void draw_circle(SDL_Renderer *renderer, int c_x, int c_y, int r) {
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrt(r * r - y * y);
        for (int x = -dx; x <= dx; x++) {
            SDL_RenderDrawPoint(renderer, c_x + x, c_y + y);
        }
    }
}

// Distance between (x1,y1) and (x2,y2)
int distance(int x1, int y1, int x2, int y2 ) {
    return (int)sqrt(pow((double)(x2 - x1), 2.0) + pow((double)(y2 - y1), 2.0));
}