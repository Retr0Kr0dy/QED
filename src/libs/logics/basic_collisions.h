extern int CHAR_SIZE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int GRID_WIDTH;
extern int GRID_HEIGHT;

extern Voidness voidness_list[256 * 240];
extern Electron electrons[256];


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



void handleVoidness(unsigned int *array, float dt, int numElectrons) {
    // Allocate and initialize temporary array
    Voidness *newGrid = (Voidness *)malloc(GRID_WIDTH * GRID_HEIGHT * sizeof(Voidness));
    if (!newGrid) {
        fprintf(stderr, "Memory allocation failed. newGrid\n");
        return;
    }
    
    Electron *newElectrons = (Electron *)malloc(numElectrons * sizeof(Electron));
    if (!newElectrons) {
        fprintf(stderr, "Memory allocation failed. newElectrons\n");
        return;
    }
    
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        newGrid[i].Ex = grid[i].Ex;
        newGrid[i].Ey = grid[i].Ey;
        newGrid[i].Bz = grid[i].Bz;
        newGrid[i].epsilon = grid[i].epsilon;
        newGrid[i].mu = grid[i].mu;
    }

    for (int i = 0; i < numElectrons; i++) {
        newElectrons[i].charge = electrons[i].charge;
        newElectrons[i].frequency = electrons[i].frequency;
        newElectrons[i].posX = electrons[i].posX;
        newElectrons[i].posY = electrons[i].posY;
        newElectrons[i].vx = electrons[i].vx;
        newElectrons[i].vy = electrons[i].vy;
    }

    // Step 1: Apply perturbations from each electron
    // Step 2: Update the electric and magnetic fields



    // Constants for the simulation
    const float ke = 8.99e9f; // Coulomb's constant (in vacuum)
    const float maxInfluenceRadius = 256.0f; // Limit interaction radius for performance reasons




    // // Step 1: Calculate electron movement and update positions
    // for (int i = 0; i < numElectrons; i++) {
    //     // Update electron position based on velocity
    //     newElectrons[i].posX += newElectrons[i].vx * dt;
    //     newElectrons[i].posY += newElectrons[i].vy * dt;

    //     // Make sure electrons stay within bounds (simple boundary condition)
    //     if (newElectrons[i].posX < 0) newElectrons[i].vx = - newElectrons[i].vx;
    //     if (newElectrons[i].posX >= GRID_WIDTH) newElectrons[i].vx = - newElectrons[i].vx;
    //     if (newElectrons[i].posY < 0) newElectrons[i].vy = - newElectrons[i].vy;
    //     if (newElectrons[i].posY >= GRID_HEIGHT) newElectrons[i].vy = - newElectrons[i].vy;

    //     // For each electron, calculate its influence on the surrounding grid points based on Coulomb's Law
    //     int gridX = (int)newElectrons[i].posX;
    //     int gridY = (int)newElectrons[i].posY;

    //     // Loop over nearby grid points within the maxInfluenceRadius
    //     for (int offsetY = -maxInfluenceRadius; offsetY <= maxInfluenceRadius; offsetY++) {
    //         for (int offsetX = -maxInfluenceRadius; offsetX <= maxInfluenceRadius; offsetX++) {
    //             int neighborX = gridX + offsetX;
    //             int neighborY = gridY + offsetY;

    //             // Ensure the neighbor point is within the grid bounds
    //             if (neighborX < 0 || neighborX >= GRID_WIDTH || neighborY < 0 || neighborY >= GRID_HEIGHT) {
    //                 continue;
    //             }

    //             // Calculate the distance from the electron to the grid point
    //             float dx = neighborX - newElectrons[i].posX;
    //             float dy = neighborY - newElectrons[i].posY;
    //             float distanceSquared = dx * dx + dy * dy;

    //             // Avoid division by zero (skip self-interaction or very close points)
    //             if (distanceSquared < 0.01f) {
    //                 continue;
    //             }

    //             // Apply Coulomb's law: E = k_e * q / r^2
    //             float distance = sqrtf(distanceSquared);
    //             if (distance > maxInfluenceRadius) {
    //                 continue; // Ignore points outside of the influence radius
    //             }

    //             float electricFieldMagnitude = ke * newElectrons[i].charge / distanceSquared;

    //             // Calculate the electric field components in x and y directions
    //             float electricFieldX = electricFieldMagnitude * (dx / distance);
    //             float electricFieldY = electricFieldMagnitude * (dy / distance);

    //             int neighborIdx = neighborY * GRID_WIDTH + neighborX;

    //             // Accumulate electric field in the new grid (affecting neighboring grid points)
    //             newGrid[neighborIdx].Ex += electricFieldX;
    //             newGrid[neighborIdx].Ey += electricFieldY;

    //             // Update local charge density (assuming electron deposit some charge)
    //             newGrid[neighborIdx].chargeDensity += newElectrons[i].charge / (distanceSquared);
    //         }
    //     }
    // }

    // Step 1: Calculate electron movement and update positions
    for (int i = 0; i < numElectrons; i++) {
        // Update electron position based on velocity
        newElectrons[i].posX += newElectrons[i].vx * dt;
        newElectrons[i].posY += newElectrons[i].vy * dt;

        // Make sure electrons stay within bounds (simple boundary condition)
        if (newElectrons[i].posX < 0) newElectrons[i].posX = 0;
        if (newElectrons[i].posX >= GRID_WIDTH) newElectrons[i].posX = GRID_WIDTH - 1;
        if (newElectrons[i].posY < 0) newElectrons[i].posY = 0;
        if (newElectrons[i].posY >= GRID_HEIGHT) newElectrons[i].posY = GRID_HEIGHT - 1;

        // For each electron, calculate its influence on the surrounding grid points based on Coulomb's Law
        int gridX = (int)newElectrons[i].posX;
        int gridY = (int)newElectrons[i].posY;

        // Loop over nearby grid points within the maxInfluenceRadius
        for (int offsetY = -maxInfluenceRadius; offsetY <= maxInfluenceRadius; offsetY++) {
            for (int offsetX = -maxInfluenceRadius; offsetX <= maxInfluenceRadius; offsetX++) {
                int neighborX = gridX + offsetX;
                int neighborY = gridY + offsetY;

                // Ensure the neighbor point is within the grid bounds
                if (neighborX < 0 || neighborX >= GRID_WIDTH || neighborY < 0 || neighborY >= GRID_HEIGHT) {
                    continue;
                }

                // Calculate the distance from the electron to the grid point
                float dx = neighborX - newElectrons[i].posX;
                float dy = neighborY - newElectrons[i].posY;
                float distanceSquared = dx * dx + dy * dy;

                // Avoid division by zero (skip self-interaction or very close points)
                if (distanceSquared < 0.01f) {
                    continue;
                }

                // Apply Coulomb's law: E = k_e * q / r^2
                float distance = sqrtf(distanceSquared);
                if (distance > maxInfluenceRadius) {
                    continue; // Ignore points outside of the influence radius
                }

                float electricFieldMagnitude = ke * newElectrons[i].charge / distanceSquared;

                // Calculate the electric field components in x and y directions
                float electricFieldX = electricFieldMagnitude * (dx / distance);
                float electricFieldY = electricFieldMagnitude * (dy / distance);

                int neighborIdx = neighborY * GRID_WIDTH + neighborX;

                // Accumulate electric field in the new grid (affecting neighboring grid points)
                newGrid[neighborIdx].Ex += electricFieldX;
                newGrid[neighborIdx].Ey += electricFieldY;

                // Update local charge density (assuming electron deposit some charge)
                newGrid[neighborIdx].chargeDensity += newElectrons[i].charge / (distanceSquared);
            }
        }

        // Step 1b: Apply Lorentz force to update electron velocity
        int electronIdx = (int)newElectrons[i].posY * GRID_WIDTH + (int)newElectrons[i].posX;
        // Get the local fields at the electron's position
        float Ex = newGrid[electronIdx].Ex;
        float Ey = newGrid[electronIdx].Ey;
        float Bz = newGrid[electronIdx].Bz;

        // Lorentz force: F = q(E + v × B)
        // Update velocity due to electric field: a_x = (q / m) * Ex, a_y = (q / m) * Ey
        // For simplicity, assume mass m = 1
        newElectrons[i].vx += (newElectrons[i].charge * Ex) * dt;
        newElectrons[i].vy += (newElectrons[i].charge * Ey) * dt;

        // Update velocity due to magnetic field: |v_x'| = |v_y * Bz|, |v_y'| = |v_x * Bz|
        float lorentzForceX = fabs(newElectrons[i].vy) * fabs(Bz); // Take absolute value for force calculation
        float lorentzForceY = fabs(newElectrons[i].vx) * fabs(Bz); // Same for vy

        // Apply Lorentz force while preserving the original velocity signs
        newElectrons[i].vx += (newElectrons[i].vx < 0 ? -lorentzForceX : lorentzForceX) * dt;
        newElectrons[i].vy += (newElectrons[i].vy < 0 ? -lorentzForceY : lorentzForceY) * dt;
    }

    // Step 2: Calculate electromagnetic field propagation using Maxwell's equations
    for (int y = 1; y < GRID_HEIGHT - 1; y++) {
        for (int x = 1; x < GRID_WIDTH - 1; x++) {
            int idx = y * GRID_WIDTH + x;
            int idxUp = (y + 1) * GRID_WIDTH + x;
            int idxDown = (y - 1) * GRID_WIDTH + x;
            int idxLeft = y * GRID_WIDTH + (x - 1);
            int idxRight = y * GRID_WIDTH + (x + 1);

            // Faraday's Law: ∇ × E = -∂B/∂t (updates electric field based on magnetic field)
            float dBz_dx = (grid[idxRight].Bz - grid[idxLeft].Bz) / (2 * GRID_WIDTH);  // Approximate ∂Bz/∂x
            float dBz_dy = (grid[idxUp].Bz - grid[idxDown].Bz) / (2 * GRID_WIDTH);     // Approximate ∂Bz/∂y

            newGrid[idx].Ex += dt * grid[idx].mu * dBz_dy;
            newGrid[idx].Ey += dt * grid[idx].mu * (-dBz_dx);

            // Ampère's Law: ∇ × B = μ₀ (J + ε₀ ∂E/∂t) (updates magnetic field based on electric field)
            float dEy_dx = (grid[idxRight].Ey - grid[idxLeft].Ey) / (2 * GRID_WIDTH);  // Approximate ∂Ey/∂x
            float dEx_dy = (grid[idxUp].Ex - grid[idxDown].Ex) / (2 * GRID_WIDTH);     // Approximate ∂Ex/∂y

            newGrid[idx].Bz += dt * (dEy_dx - dEx_dy) / grid[idx].epsilon;
        }
    }

    // copy updated value to electrons list
    for (int i = 0; i < numElectrons; i++) {
        electrons[i] = newElectrons[i];
    }

    // Step 3: Copy the updated values back to the original grid and update visualization
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y * GRID_WIDTH + x] = newGrid[y * GRID_WIDTH + x];
            
            // Visualization
            float electricFieldMagnitude = sqrtf(grid[y * GRID_WIDTH + x].Ex * grid[y * GRID_WIDTH + x].Ex +
                                                  grid[y * GRID_WIDTH + x].Ey * grid[y * GRID_WIDTH + x].Ey);
            Uint8 electricColor = (Uint8)electricFieldMagnitude % 255;  // Cap at 255

            float magneticFieldMagnitude = fabsf(grid[y * GRID_WIDTH + x].Bz);
            Uint8 magneticColor = (Uint8)magneticFieldMagnitude % 255;  // Cap at 255

            Uint8 red = electricColor;
            Uint8 green = 0;
            Uint8 blue = magneticColor;

            // Check if an electron is at this position
            for (int e = 0; e < numElectrons; e++) {
                int electronX = (int)roundf(electrons[e].posX);
                int electronY = (int)roundf(electrons[e].posY);
                if (x == electronX && y == electronY) {
                    red = 0;  // Color the electron as red
                    green = 255;
                    blue = 0;
                }
            }

            // Store the color in the SDL array (BRGA format)
            array[y * GRID_WIDTH + x] = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), 255, red, green, blue);
        }
    }


    // Free temporary arrays
    free(newGrid);
    free(newElectrons);
}
