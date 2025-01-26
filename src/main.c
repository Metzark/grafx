#pragma region Include

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>

#pragma endregion Include

#pragma region Macro

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#pragma endregion Macros

#pragma region Declare

struct GRFX_GUI {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int running;
};

// Initialize SDL Library
void GRFX_Init();

// Destroy renderer and window and quite SDL
void GRFX_End(struct GRFX_GUI *gui);

// Create a GUI initialized with a window and renderer
struct GRFX_GUI GRFX_Create_GUI();

// Clear the renderer with a color
void GRFX_Clear_GUI(struct GRFX_GUI *gui);

// Draws a filled in circle with radius r, centered at (c_x, c_y)
void GRFX_Draw_Circle(SDL_Renderer *renderer, int centerX, int centerY, int radius);

// Distance between (x1,y1) and (x2,y2)
int MAF_Distance(int x1, int y1, int x2, int y2 );

#pragma endregion Declare

int main(int argc, char *argv[]) {

    // Initialize SDL
    GRFX_Init();

    // Create GUI with window and renderer
    struct GRFX_GUI gui = GRFX_Create_GUI();

    SDL_Event event;

    int dragging = 0;
    int startX = 0, startY = 0;

    int center_x = WINDOW_WIDTH / 2 - 25;
    int center_y = WINDOW_HEIGHT / 2 - 25;

    while (gui.running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    gui.running = false;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if(MAF_Distance(center_x, center_y, event.button.x, event.button.y) < 50) {
                            dragging = 1;
                            startX = event.button.x - center_x;
                            startY = event.button.y - center_y;
                        }
                    }
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (dragging) {
                        center_x = event.motion.x - startX;
                        center_y = event.motion.y - startY;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT && dragging) {
                        dragging = 0;
                    }
                    break;
                default:
                    break;
            }
        }

        // Clear GUI before rendering next frame
        GRFX_Clear_GUI(&gui);

        // Set color
        SDL_SetRenderDrawColor(gui.renderer, 255, 255, 0, 255);

        // Draw circle
        GRFX_Draw_Circle(gui.renderer, center_x, center_y, 50);

        // Present the renderer (show rendered content on screen)
        SDL_RenderPresent(gui.renderer);

        // Limit frame rate (approx. 60 FPS)
        SDL_Delay(16);
    }

    // End
    GRFX_End(&gui);    

    return 0;
}

#pragma region GRFX Def

void GRFX_Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }
}

void GRFX_End(struct GRFX_GUI *gui){
    SDL_DestroyRenderer(gui->renderer);

    SDL_DestroyWindow(gui->window);

    SDL_Quit();
}

struct GRFX_GUI GRFX_Create_GUI() {
    struct GRFX_GUI new_gui;

    // Create an SDL window
    new_gui.window = SDL_CreateWindow(
        "Grafx",           
        WINDOW_WIDTH,                     
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
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
        NULL           
    );

    // If renderer is null, exit
    if (new_gui.renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(new_gui.window);
        SDL_Quit();
        exit(1);
    }

    // Setting gui as 'running'
    new_gui.running = true;

    return new_gui;
}

void GRFX_Clear_GUI(struct GRFX_GUI *gui) {
    SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 0);
    SDL_RenderClear(gui->renderer);
}

void GRFX_Draw_Circle(SDL_Renderer *renderer, int c_x, int c_y, int r) {
    for (int y = -r; y <= r; y++) {
        int dx = (int)sqrt(r * r - y * y);
        for (int x = -dx; x <= dx; x++) {
            SDL_RenderPoint(renderer, c_x + x, c_y + y);
        }
    }
}

#pragma endregion GRFX Def

#pragma region MAF Def

int MAF_Distance(int x1, int y1, int x2, int y2 ) {
    return (int)sqrt((double)(x2 - x1) * (x2 -x1) + (y2 - y1) * (y2 - y1));
}

#pragma endregion MAF Def