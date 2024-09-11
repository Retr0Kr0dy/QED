extern int CHAR_SIZE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;
extern int ELEMENT_SIZE;


extern char *FONT_1;
extern char *FONT_2;
extern char *FONT_3;

#include <string.h>

extern int CHAR_WIDTH;
extern int CHAR_HEIGHT;


SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 255};

SDL_Color blue = {255, 0, 0, 255};
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {0, 0, 255, 255};
SDL_Color weird = {128, 0, 56, 255};

uint32_t SDcolor2uint32(SDL_Color color) {
    return (color.b << 24) | (color.b << 16) | (color.b << 8) | color.b; // AARRGGBB
}

void drawMir(unsigned int *pixels) {
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i) {
        unsigned char r = rand() % 256;
        unsigned char g = rand() % 256;
        unsigned char b = rand() % 256;
        pixels[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}

void drawPotMir(unsigned int *pixels) {
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i) {
        unsigned char r = rand() % 256;
        unsigned char g = 0;
        unsigned char b = rand() % 256;
        pixels[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}

// Function to draw a pixel in the pixels array
void drawPixel(int x, int y, Uint32 color, unsigned int* pixels) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_WIDTH) {
        pixels[y * GRID_WIDTH + x] = color;
    }
}

// Function to draw a filled circle
void drawCircle(int centerX, int centerY, int radius, Uint32 color, unsigned int* pixels) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                drawPixel(centerX + x, centerY + y, color, pixels);
            }
        }
    }
}



void renderText(const char* text, int CHAR_SIZE , char *font_name, SDL_Color color, int x, int y, unsigned int* pixels) {
    // Load font
    TTF_Font *font = TTF_OpenFont(font_name, CHAR_SIZE);  // Change the path to your font file
    if (!font) {
        printf("TTF_OpenFont failed: %s\n", TTF_GetError());
        return;
    }

    // Render the text to an SDL surface
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    if (!textSurface) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    // Get the text surface dimensions
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Create a new SDL surface in the format of the screen
    SDL_Surface* formattedSurface = SDL_CreateRGBSurfaceWithFormat(0, textWidth, textHeight, 32, SDL_PIXELFORMAT_RGBA32);
    if (!formattedSurface) {
        printf("Unable to create formatted surface! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    // Blit the text surface onto the formatted surface
    SDL_BlitSurface(textSurface, NULL, formattedSurface, NULL);
    SDL_FreeSurface(textSurface);

    // Lock the formatted surface for raw pixel access
    SDL_LockSurface(formattedSurface);

    // Get the pixel data from the surface
    Uint32* surfacePixels = (Uint32*)formattedSurface->pixels;

    if (x == -1) { x = (GRID_WIDTH / 2) - (textWidth / 2); }

    // Iterate over each pixel in the text surface and modify the corresponding pixel in the output array
    for (int j = 0; j < textHeight; ++j) {
        for (int i = 0; i < textWidth; ++i) {
            int srcIndex = j * textWidth + i;
            int dstIndex = (y + j) * GRID_WIDTH + (x + i);

            if (x + i < 0 || x + i >= GRID_WIDTH || y + j < 0 || y + j >= GRID_HEIGHT) {
                continue; // Skip pixels that are out of bounds
            }

            Uint32 pixel = surfacePixels[srcIndex];
            Uint8 alpha = pixel >> 24; // Extract the alpha value

            // // Only update the pixel if it is not fully transparent
            // if (alpha > 200) {
            //     pixels[dstIndex] = pixel;
            // }

            if (alpha > 0) { // Only blend if the pixel is not fully transparent
                Uint8 srcR = (pixel >> 16) & 0xFF;
                Uint8 srcG = (pixel >> 8) & 0xFF;
                Uint8 srcB = pixel & 0xFF;

                Uint32 dstPixel = pixels[dstIndex];
                Uint8 dstR = (dstPixel >> 16) & 0xFF;
                Uint8 dstG = (dstPixel >> 8) & 0xFF;
                Uint8 dstB = dstPixel & 0xFF;
                Uint8 dstA = (dstPixel >> 24) & 0xFF;

                Uint8 outR = (srcR * alpha + dstR * (255 - alpha)) / 255;
                Uint8 outG = (srcG * alpha + dstG * (255 - alpha)) / 255;
                Uint8 outB = (srcB * alpha + dstB * (255 - alpha)) / 255;
                Uint8 outA = alpha + (dstA * (255 - alpha)) / 255;

                pixels[dstIndex] = (outA << 24) | (outR << 16) | (outG << 8) | outB;
            }
        }
    }

    // Unlock and free the formatted surface
    SDL_UnlockSurface(formattedSurface);
    SDL_FreeSurface(formattedSurface);
    TTF_CloseFont(font);
}
