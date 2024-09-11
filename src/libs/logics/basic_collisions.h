extern int CHAR_SIZE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;

extern struct voidness voidness_list[256 * 240];

// void handleEntityLogic (struct electron *Character) {
//     // Character->posX = Character->posX + Character->velX;
//     // Character->posY = Character->posY + Character->velY;
//     Character->posX = Character->posX + Character->velX;
//     Character->posY = Character->posY + Character->velY;

//     if (Character->posX < 0) {Character->posX + 1;}
//     if (Character->posX > SCREEN_WIDTH) {Character->posX - 1;}
//     if (Character->posY < 0) {Character->posX + 1;}
//     if (Character->posY > SCREEN_HEIGHT) {Character->posX - 1;}
// }


// unsigned int getColorFromPotential(float ratio) {
//     if (ratio < 0.0f) {
//         ratio = 0.0f;
//     } else if (ratio > 1.0f) {
//         ratio = 1.0f;
//     }

//     uint8_t R = (uint8_t)(ratio * 255);          
//     uint8_t G = 0;                               
//     uint8_t B = (uint8_t)((1.0f - ratio) * 255); 

//     unsigned int color = (0xFF << 24) | (R << 16) | (G << 8) | B;

//     return color;
// }




// unsigned int getColorFromPotential(float potential) {
//     // Assumes potential is between 0 and 1. Adjust accordingly if it's not:
//     int r = (int)(potential * 255);  // maps to [0, 255]
//     int b = 255 - r;                 // simple gradient
//     return (r << 16) | (0 << 8) | b; // Color format (RGB)
// }

unsigned int getColorFromPotential(float ratio) {
    // Clamp the value to the range [0.0, 1.0]
    if (ratio < 0.0f) {
        ratio = 0.0f;
    } else if (ratio > 1.0f) {
        ratio = 1.0f;
    }

    // Calculate the RGB components
    unsigned int red = (unsigned int)(ratio * 255.0f);   // 0.0 -> 0, 1.0 -> 255
    unsigned int green = 0;                                 // Always 0
    unsigned int blue = (unsigned int)((1.0f - ratio) * 255.0f); // 0.0 -> 255, 1.0 -> 0

    // Convert to unsigned int (assuming little-endian color format)
    return (red << 16) | (green << 8) | blue;
}


// // Helper function to calculate ratio from pixel (already defined)
// float gradient_ratio(unsigned int pixel) {
//     uint8_t R = (pixel >> 16) & 0xFF; // Red component
//     uint8_t G = (pixel >> 8) & 0xFF;  // Green component
//     uint8_t B = pixel & 0xFF;         // Blue component
//     float maxColor = (float)(R + B);
//     if (maxColor == 0.0f) return 0.0f; // Handle case of no red/blue

//     return (float)R / maxColor; // Calculate ratio
// }

float gradient_ratio(unsigned int pixel) {
    uint8_t R = (pixel >> 16) & 0xFF; // comment about R extraction 
    uint8_t B = pixel & 0xFF;         // comment about B extraction 
    float total = (float)(R + B);

    if (total == 0.0f) return 0.0f;
    return R / total; // return red component ratio 
}


float calculate_average(float* ratios, int count) {
    if (count == 0) {
        return 0.0; // Éviter la division par zéro si le count est zéro
    }

    float sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += ratios[i]; // Addition des éléments du tableau
    }

    return sum / count; // Renvoie la moyenne
}

// Function to compute the median gradient effect of surrounding pixels
float median_surrounding_ratio(unsigned int* pixels, int x, int y) {
    // Directions for surrounding pixels (8 neighbors)
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, 
        {0, -1},           {0, 1}, 
        {1, -1}, {1, 0}, {1, 1}
    };

    float ratios[8];
    int count = 0;

    for (int i = 0; i < 8; i++) {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];

        if (nx >= 0 && ny >= 0 && nx < GRID_WIDTH && ny < GRID_HEIGHT) {
            unsigned int pixel_color = pixels[ny * GRID_WIDTH + nx]; // Access pixel in the image
            float ratio = gradient_ratio(pixel_color); // Calculate ratio
            ratios[count++] = ratio;
        } else {}
    }

    if (count == 0) return 0.0f;

    // Calculate and return the median of the ratios
    return calculate_average(ratios, count);
}
















float dampling_factor = 100.0f;

void handleVoidPotential (unsigned int* pixels) {
    unsigned int new_pixels[GRID_WIDTH * GRID_HEIGHT];

    // get info on map

    for (int y=0; y < GRID_HEIGHT; ++y ) {
        for (int x=0; x < GRID_WIDTH; ++x ) {
            struct voidness *point = &voidness_list[y*GRID_WIDTH+x];

            float potential = gradient_ratio(pixels[y*GRID_WIDTH+x]);
            float median = median_surrounding_ratio(pixels, x, y);

            point->potential = potential;
            point->surrounding = median; 
        }
    }

    // use map info

    for (int y=0; y < GRID_HEIGHT; ++y ) {
        for (int x=0; x < GRID_WIDTH; ++x ) {
            struct voidness point = voidness_list[y*GRID_WIDTH+x];

            if (point.surrounding != 0.0f) {
                //unsigned int color = getColorFromPotential(point.potential*point.surrounding/2);
//                unsigned int color = getColorFromPotential((point.potential + (point.surrounding * dampling_factor)) / (1.0f + dampling_factor));
/*
                // Define a suitable damping and influence factor
                float dampingFactor = 0.01f;
                float influenceFactor = 2.0f;

                // Updating potential with adjusted factors
                float newPotential = (point.potential + (point.surrounding * influenceFactor)) / (1.0f + influenceFactor);

                // Optional: Apply damping to the new potential
                newPotential *= (1.0f - dampingFactor);

                // Clamp new potential to the range [0.0, 1.0]
                if (newPotential < 0.0f) newPotential = 0.0f;
                if (newPotential > 1.0f) newPotential = 1.0f;

                // Update the color based on new potential

                unsigned int color = getColorFromPotential(newPotential);
*/
                // Define a suitable damping and influence factor
                float dampingFactor = 0.01f;
                float influenceFactor = 2.0f;

                // Calculate the weighted contribution from surrounding pixels
                // Assuming you have access to surrounding pixels (left, right, above, below)
                float totalSurrounding = point.surrounding*8;
                float neighborCount = 8.0; // Number of surrounding neighbors considered

                // More sophisticated averaging
                float newPotential = (point.potential + (totalSurrounding / neighborCount) * influenceFactor) / (1.0f + influenceFactor);

                // Apply exponential damping based on the current potential
                newPotential *= exp(-dampingFactor * newPotential); // Exponential decay

                // Clamp new potential to the range [0.0, 1.0]
                newPotential = fmax(0.0f, fmin(newPotential, 1.0f)); // Clamping syntax

                // Update the color based on new potential
                unsigned int color = getColorFromPotential(newPotential);

                pixels[y*GRID_WIDTH+x] = color;            
            }
        }
    }

}