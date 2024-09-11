extern int CHAR_SIZE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;

extern struct voidness voidness_list[256 * 240];

unsigned int getColorFromPotential(float ratio) {
    if (ratio < 0.0f) {
        ratio = 0.0f;
    } else if (ratio > 1.0f) {
        ratio = 1.0f;
    }

    unsigned int red = (unsigned int)(ratio * 255.0f);   // 0.0 -> 0, 1.0 -> 255
    unsigned int green = 0;                                 // Always 0
    unsigned int blue = (unsigned int)((1.0f - ratio) * 255.0f); // 0.0 -> 255, 1.0 -> 0

    return (red << 16) | (green << 8) | blue;
}

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












extern float DF;
extern float IF;
extern float TS;
extern int VN;












void handleVoidPotential (unsigned int* pixels) {
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

                // // Define a suitable damping and influence factor
                // float dampingFactor = 0.01f;
                // float influenceFactor = 2.0f;



                // // Calculate the weighted contribution from surrounding pixels
                // // Assuming you have access to surrounding pixels (left, right, above, below)
                // float totalSurrounding = point.surrounding*8;
                // float neighborCount = 8.0; // Number of surrounding neighbors considered

                // // More sophisticated averaging
                // float newPotential = (point.potential + (totalSurrounding / neighborCount) * influenceFactor) / (1.0f + influenceFactor);

                // // Apply exponential damping based on the current potential
                // newPotential *= exp(-dampingFactor * newPotential); // Exponential decay

                // // Clamp new potential to the range [0.0, 1.0]
                // newPotential = fmax(0.0f, fmin(newPotential, 1.0f)); // Clamping syntax

                // // Update the color based on new potential
                // unsigned int color = getColorFromPotential(newPotential);

                // pixels[y*GRID_WIDTH+x] = color;










                // float dampingFactor = 0.1f;
                // float influenceFactor = 2.0f;
                // float totalSurrounding = 4.0f;
                // int validNeighbors = 4;


                float dampingFactor = DF;
                float influenceFactor = IF;
                float totalSurrounding = TS;
                int validNeighbors = VN;

                // Traverse the 8 surrounding pixels
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dy == 0 && dx == 0) continue; // Skip the central pixel
                        int newX = x + dx;
                        int newY = y + dy;
                        if (newX >= 0 && newX < GRID_WIDTH && newY >= 0 && newY < GRID_HEIGHT) {
                            struct voidness neighbor = voidness_list[newY * GRID_WIDTH + newX];
                            totalSurrounding += neighbor.potential;
                            validNeighbors++;
                        }
                    }
                }

                // Update the potential influenced by surrounding pixels
                if (validNeighbors > 0) {
                    float averageSurrounding = totalSurrounding / validNeighbors;
                    float newPotential = point.potential + (averageSurrounding - point.potential) * influenceFactor;

                    // Apply damping to reduce sharp changes
                    newPotential *= exp(-dampingFactor);

                    // Clamp new potential to the range [0.0, 1.0]
                    newPotential = fmax(0.0f, fmin(newPotential, 1.0f)); 
                    
                    // Update the color based on new potential
                    unsigned int color = getColorFromPotential(newPotential);
                    pixels[y * GRID_WIDTH + x] = color;

                    // Update the point's potential for next iteration
                    point.potential = newPotential;

















            }}
        }
    }

}