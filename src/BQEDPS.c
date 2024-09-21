#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "libs/entities.h"
#include "libs/graphics/basic_bitmap.h"
#include "libs/logics/basic_collisions.h"

int CHAR_SIZE = 8;
int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 960;
int GRID_WIDTH = 256;
int GRID_HEIGHT = 240;
int ELEMENT_SIZE = 8;

int MULTIPLIER = 8;

char *FONT_1 = "assets/fonts/font_1.ttf";
char *FONT_2 = "assets/fonts/font_2.ttf";
char *FONT_3 = "assets/fonts/font_3.ttf";



float DF = 0.1f;
float IF = 2.0f;
float TS = 4.0f;
int VN = 4;


int main(int argc, char* argv[]) {
    if (argc < 1) {
        printf("Bad usage: %s <none> ...\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("mire", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(1);  // Enable vsync

    // Initialize OpenGL settings
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GRID_WIDTH, GRID_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // OpenGL texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    unsigned int* pixels = (unsigned int*)malloc(GRID_WIDTH * GRID_HEIGHT * sizeof(unsigned int));
    if (!pixels) {
        SDL_Log("Failed to allocate memory for pixel buffer");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    memset(pixels, 0, GRID_WIDTH * GRID_HEIGHT * sizeof(unsigned int));

    // Main loop
    int running = 1;
    int debug = 0;
    int pause = 0;


    float timeRatio = 1.0f;
    int numElectrons = 0;

    SDL_Event event;
    Uint32 startTime = SDL_GetTicks();
    //Uint32 origStartTime = startTime;
    float fps = 0;
    int frameCount = 0;
    char debugText[256];
    //char debugText2[256];


    // Initialize the grid
    initializeVoidnessGrid(grid);

//    addElectrons(numElectrons, GRID_WIDTH/2,  GRID_HEIGHT/2, 1.0f, 0.0f, 0.0f, 5.0f);
//    drawPotMir(pixels);


    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        running = 0;
                        break;
                    case SDLK_F1:
                        debug ^= 1;
                        break;
                    case SDLK_F2:
                        pause ^= 1;
                        break;
                    case SDLK_r:
                        resetGrid(grid);
                        break;
                    case SDLK_a:
                        addElectrons(numElectrons+1, 1.0f * (rand()%GRID_WIDTH),  1.0f * (rand()%GRID_HEIGHT), 0.70f, 1.0f, 1.0f, 0.0f);
                        numElectrons++;
                        break;
                    case SDLK_z:
                        addElectrons(numElectrons+1, 1.0f * (rand()%GRID_WIDTH),  1.0f * (rand()%GRID_HEIGHT), -0.70f, 1.0f, 1.0f, 0.0f);
                        numElectrons++;
                        break;
                    case SDLK_f:
                        applyNarrowedPerturbation(grid,rand()%GRID_WIDTH,rand()%GRID_HEIGHT,1.0f,1.0f,1.0f);
                        break;
                    case SDLK_g:
                        applyPerturbation(grid,(GRID_WIDTH/2)+5,(GRID_HEIGHT/2)+3,0.0f,0.99f);
                        break;
                    case SDLK_h:
                        int x,y;
                        x = rand()%GRID_WIDTH;
                        y = rand()%GRID_HEIGHT;
                        grid[y * GRID_WIDTH + x].Ex = 1.00f;  // Initial pulse in the x-direction
                        grid[y * GRID_WIDTH + x].Ey = 1.0f;  // Initial pulse in the y-direction
                        grid[y * GRID_WIDTH + x].Bz = 0.0f;  // Start with no magnetic field
                        break;
                    case SDLK_i:
                        // timeRatio += 0.1f;
                        timeRatio *= 10;
                        break;
                    case SDLK_k:
                        timeRatio /= 10;
                        break;
                }
            }
        }

        //Uint32 origElapsedTime = SDL_GetTicks() - origStartTime;
        //(float) origElapsedTime / timeRatio

        if (!pause) {
           handleVoidness(pixels, timeRatio, numElectrons+1);
           
           
            //handleVoidPotential(pixels);
        }



printf ("Ex:%.2f Ey:%.2f Bz:%.2f ep:%.2f mu:%.2f chde:%.2f cudeX,Y:%.2f:%.2f\n",grid[24100].Ex, grid[24100].Ey, grid[24100].Bz, grid[24100].epsilon, grid[24100].mu, grid[24100].chargeDensity, grid[24100].currentDensityX, grid[24100].currentDensityY);

        // Creating temp buffer for rendring
        unsigned int* tempPixels = (unsigned int*)malloc(GRID_WIDTH * GRID_HEIGHT * sizeof(unsigned int));

        for (int i = 0; i < GRID_HEIGHT * GRID_WIDTH; i++) {
            tempPixels[i] = pixels[i];
        }

        if (debug) {
            Uint32 elapsedTime = SDL_GetTicks() - startTime;
            // Calculate FPS
            frameCount++;
            if (elapsedTime >= 1000) {
                fps = frameCount / (elapsedTime / 1000.0f);
                startTime = SDL_GetTicks();
                frameCount = 0;
            }

            //snprintf(debugText, sizeof(debugText), "FPS: %.2f", fps);
            snprintf(debugText, sizeof(debugText), "FPS: %.1f, Time ratio %.10f", fps, timeRatio);
            
            renderText(debugText, 16, FONT_1, green, 1, 1, tempPixels);
        }


        // Update the texture with the new pixel buffer
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GRID_WIDTH, GRID_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, tempPixels);

        // Clear the screen and render the texture
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);

        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < 256.0f; ++x) {
                float tx = (float)x / (float)GRID_WIDTH;
                float ty = (float)y / (float)GRID_HEIGHT;
                float txNext = (float)(x + 1) / (float)GRID_WIDTH;
                float tyNext = (float)(y + 1) / (float)GRID_HEIGHT;

                float vx = x * 1.0f;
                float vy = y * 1.0f;
                float vxNext = (x + 1) * 1.0f;
                float vyNext = (y + 1) * 1.0f;

                glTexCoord2f(tx, ty); glVertex2f(vx, vy);
                glTexCoord2f(txNext, ty); glVertex2f(vxNext, vy);
                glTexCoord2f(txNext, tyNext); glVertex2f(vxNext, vyNext);
                glTexCoord2f(tx, tyNext); glVertex2f(vx, vyNext);
            }
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    TTF_Quit();
    glDeleteTextures(1, &texture);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
