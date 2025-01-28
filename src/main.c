#pragma region Include

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <SDL3/SDL.h>

#pragma endregion Include

#pragma region Macro

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NUM_BLOCKS 5
#define LIGHT_RADIUS 20
#define NUM_LIGHT_RAYS 30;
#define NUM_RAY_REFLECTIONS 1
#define RAY_OPACITY 50
#define M_PI 3.14159265358979323846

#pragma endregion Macros

#pragma region Declare

struct GRFX_GUI {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int running;
    struct GRFX_Light *light;
    struct SDL_FRect *blocks[NUM_BLOCKS];
};

struct GRFX_Light {
    int x;
    int y;
    int r;
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

// Render a 'ray' (or line)
void GRFX_Render_Ray(struct GRFX_GUI *gui, int x1, int y1, double dx, double dy, SDL_FRect *prev_col, int count);

// Distance between (x1,y1) and (x2,y2)
int MAF_Distance(int x1, int y1, int x2, int y2 );

#pragma endregion Declare

int main(int argc, char *argv[]) {

    // Initialize SDL
    GRFX_Init();

    // Create GUI with window and renderer
    struct GRFX_GUI gui = GRFX_Create_GUI();
    SDL_Event event;
    int dragging = -1;
    int startX = 0, startY = 0;
    int center_x = WINDOW_WIDTH / 2 - LIGHT_RADIUS;
    int center_y = WINDOW_HEIGHT / 2 - LIGHT_RADIUS;
    int num_rays = NUM_LIGHT_RAYS;
    int num_reflections = NUM_RAY_REFLECTIONS;

    while (gui.running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    gui.running = false;
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if(MAF_Distance(center_x, center_y, event.button.x, event.button.y) < LIGHT_RADIUS) {
                            dragging = NUM_BLOCKS;
                            startX = event.button.x - center_x;
                            startY = event.button.y - center_y;
                            break;
                        }

                        for (int i = 0; i < NUM_BLOCKS; i++) {
                            if(event.button.x >= gui.blocks[i]->x && event.button.x <= gui.blocks[i]->x + gui.blocks[i]->w && event.button.y >= gui.blocks[i]->y && event.button.y <= gui.blocks[i]->y + gui.blocks[i]->h) {
                                dragging = i;
                                startX = event.button.x - gui.blocks[i]->x;
                                startY = event.button.y - gui.blocks[i]->y;
                            }
                        }
                    }
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    if (dragging == NUM_BLOCKS) {
                        center_x = event.motion.x - startX;
                        center_y = event.motion.y - startY;
                    }

                    if (dragging >= 0 && dragging < NUM_BLOCKS) {
                        gui.blocks[dragging]->x = event.motion.x - startX;
                        gui.blocks[dragging]->y = event.motion.y - startY;
                    }

                    break;
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT && dragging != -1) {
                        dragging = -1;
                    }
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                        num_rays += event.wheel.y * 5;
                        if (num_rays < 2) {
                            num_rays == 2;
                        }
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (strcmp(SDL_GetKeyName(event.key.key), "Up") == 0) num_reflections++;

                    if (strcmp(SDL_GetKeyName(event.key.key), "Down") == 0) num_reflections--;

                    if (num_reflections < 1) num_reflections = 1;

                    break;
                default:
                    break;
            }
        }

        // Clear GUI before rendering next frame
        GRFX_Clear_GUI(&gui);

        SDL_SetRenderDrawColor(gui.renderer, 30, 30, 30, 255);

        for(int i = 0; i < NUM_BLOCKS; i++) {
            SDL_RenderFillRect(gui.renderer, gui.blocks[i]);
        }

        SDL_SetRenderDrawBlendMode(gui.renderer, SDL_BLENDMODE_BLEND);

        double rad = 0, dx, dy;
        float x1, y1, x2, y2;
        float r = 255, g = 0, b = 0;
        float dc = 255 * 6 / num_rays;

        for (int i = 0; i < num_rays; i++) {
            dx = cos(rad);
            dy = sin(rad);

            // Set color
            SDL_SetRenderDrawColor(gui.renderer, r, g, b, RAY_OPACITY);

            GRFX_Render_Ray(&gui, center_x, center_y, dx, dy, NULL, num_reflections);

            // Increment the radians by 2PI / Number of rays
            rad += 2 * M_PI / num_rays;

            if (r == 255 && g < 255 && b == 0) {
                g += dc;
                if (g > 255) g = 255;
                continue;
            }

            if (r > 0 && g == 255 && b == 0) {
                r -= dc;
                if (r < 0) r = 0;
                continue;
            }

            if (r == 0 && g == 255 && b < 255) {
                b += dc;
                if (b > 255) b = 255;
                continue;
            }

            if (r == 0 && g > 0 && b == 255) {
                g -= dc;
                if (g < 0) g = 0;
                continue;
            }

            if (r < 255 && g == 0 && b == 255) {
                r += dc;
                if (r > 255) r = 255;
                continue;
            }

            if (r == 255 && g == 0 && b > 0) {
                b -= dc;
                if (b < 0) b = 0;
            }
        }

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
    // Free blocks
    for (int i = 0; i < NUM_BLOCKS; i++) {
        free(gui->blocks[i]);
    }

    // Free light
    free(gui->light);
    
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


    new_gui.light = malloc(sizeof(struct GRFX_Light));
    new_gui.light->r = LIGHT_RADIUS;
    new_gui.light->x = WINDOW_WIDTH / 2 - LIGHT_RADIUS;
    new_gui.light->y = WINDOW_HEIGHT / 2 - LIGHT_RADIUS;
    
    // Create some blocks
    for (int i = 0; i < NUM_BLOCKS; i++) {
        new_gui.blocks[i] = malloc(sizeof(struct SDL_FRect));
        new_gui.blocks[i]->x = i * 100;
        new_gui.blocks[i]->y = 0;
        new_gui.blocks[i]->w = 60;
        new_gui.blocks[i]->h = 60;
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

void GRFX_Render_Ray(struct GRFX_GUI *gui, int x1, int y1, double dx, double dy, SDL_FRect *prev_col, int count) {

    if (count < 1) return;

    float x2, y2;
    double new_dx = dx, new_dy = dy;
    float max_width = WINDOW_WIDTH, max_height = WINDOW_HEIGHT;
    const double epsilon = 0.001;

    SDL_FRect *box;
    float temp, t_near, t_far;
    float x_min, x_max, y_min, y_max;
    float t_x1, t_y1, t_x2, t_y2;
    SDL_FRect *box_collision = NULL;
    float box_collision_dist = INFINITY;

    // Check for box collisions
    for (int i = 0; i < NUM_BLOCKS; i++) {
        box = gui->blocks[i];

        if (box == prev_col) continue;

        x_min = box->x, x_max = box->x + box->w, y_min = box->y, y_max = box->y + box->h;
        t_x1 = (x_min - x1) / dx;
        t_x2 = (x_max - x1) / dx;
        t_y1 = (y_min - y1) / dy;
        t_y2 = (y_max - y1) / dy;

        if (t_x1 > t_x2) {
            temp = t_x1; t_x1 = t_x2; t_x2 = temp;
        }

        if (t_y1 > t_y2) {
            temp = t_y1; t_y1 = t_y2; t_y2 = temp;
        }

        t_near = fmaxf(t_x1, t_y1);
        t_far = fminf(t_x2, t_y2);

        if (t_near <= t_far && t_far >= 0) {
            temp = MAF_Distance(x1, y1, x1 + t_near * dx, y1 + t_near * dy);
            if (temp < box_collision_dist) {
                box_collision = gui->blocks[i];
                box_collision_dist = temp;
                x2 = x1 + t_near * dx;
                y2 = y1 + t_near * dy;
            }
        }
    }

    if (box_collision != NULL && count > 1) {
        // Bottom side collision
        if (fabs(y2 - (box_collision->y + box_collision->h)) < epsilon) {
            new_dy = -dy;
            new_dx = dx;
        }
        // Top side collision
        else if (fabs(y2 - box_collision->y) < epsilon) {
            new_dy = -dy;
            new_dx = dx;
        }
        // Left side collision
        else if (fabs(x2 - box_collision->x) < epsilon) {
            new_dx = -dx;
            new_dy = dy;
        }
        // Right side collision
        else if (fabs(x2 - (box_collision->x + box_collision->w)) < epsilon) {
            new_dx = -dx;
            new_dy = dy;
        }
    }

    // If no box collisions, extend to the end of the window
    if (box_collision == NULL) {
        for (x2 = x1, y2 = y1; x2 >= 0 && x2 <= max_width && y2 >= 0 && y2 <= max_height; x2 += dx, y2 += dy);

        // Fix out of bounds values
        if (x2 < 0) x2 = 0;
        if (x2 > WINDOW_WIDTH) x2 = WINDOW_WIDTH;
        if (y2 < 0) y2 = 0;
        if (y2 > WINDOW_HEIGHT) y2 = WINDOW_HEIGHT;

        if (x2 <= 0 || x2 >= WINDOW_WIDTH) {
            new_dx = -dx;
        }

        if (y2 <= 0 || y2 >= WINDOW_HEIGHT) {
            new_dy = -dy;
        }
    }

    SDL_RenderLine(gui->renderer, x1, y1, x2, y2);

    GRFX_Render_Ray(gui, x2, y2, new_dx, new_dy, box_collision, count - 1);
}

#pragma endregion GRFX Def

#pragma region MAF Def

int MAF_Distance(int x1, int y1, int x2, int y2 ) {
    return (int)sqrt((double)(x2 - x1) * (x2 -x1) + (y2 - y1) * (y2 - y1));
}

#pragma endregion MAF Def