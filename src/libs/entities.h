extern int GRID_WIDTH;
extern int GRID_HEIGHT;

typedef struct {
    int posX, posY;  // Position of the electron on the grid
    float charge;    // Charge of the electron (typically -e)
    float vx, vy;    // Velocity in x and y directions
    float frequency; // Frequency of back and forth movement, for demonstration purpose
} Electron;

typedef struct {
    float Ex;  // Electric field in x-direction
    float Ey;  // Electric field in y-direction
    float Bz;  // Magnetic field in z-direction (perpendicular to the 2D plane)

    float epsilon;  // Permittivity of the material (optional)
    float mu;       // Permeability of the material (optional)

    float chargeDensity;  // Charge density at the point (optional)
    float currentDensityX; // Current in x-direction (optional)
    float currentDensityY; // Current in y-direction (optional)
} Voidness;


const float C = 299792458.0f;  // Speed of light in vacuum
const float DT = 1e-9f;  // Time step (adjustable)


// // Allocate memory for the grid
// Voidness *grid = (Voidness *)malloc(GRID_WIDTH * GRID_HEIGHT * sizeof(Voidness));
// TEMPORARY FIX FOR CONST
// Voidness *grid;
// grid = (Voidness *)malloc(256 * 240 * sizeof(struct voidness));


Voidness grid[256 * 240];
Electron electrons[256];


void initializeVoidnessGrid(Voidness *grid) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y * GRID_WIDTH + x].Ex = 0.0f;
            grid[y * GRID_WIDTH + x].Ey = 0.0f;
            grid[y * GRID_WIDTH + x].Bz = 0.0f;
            grid[y * GRID_WIDTH + x].epsilon = 1.0f;  // Default to vacuum permittivity
            grid[y * GRID_WIDTH + x].mu = 1.0f;       // Default to vacuum permeability
            grid[y * GRID_WIDTH + x].chargeDensity = 0.0f;
        }
    }
}


void addElectrons(int id, float x, float y, float charge, float vx, float vy, float frequency) {
    Electron electron;

    electron.posX = x;
    electron.posY = y;
    electron.charge = charge;
    electron.vx = vx;
    electron.vy = vy;
    electron.frequency = frequency;

    electrons[id] = electron;
}

void resetGrid(Voidness* grid) {
    // Reset the grid fields to zero or to initial conditions
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y * GRID_WIDTH + x].Ex = 0.0f;
            grid[y * GRID_WIDTH + x].Ey = 0.0f;
            grid[y * GRID_WIDTH + x].Bz = 0.0f;
        }
    }
}














unsigned int interpolateColor(float value, float minValue, float maxValue, unsigned int colorStart, unsigned int colorEnd) {
    // Clamp value between minValue and maxValue
    if (value < minValue) value = minValue;
    if (value > maxValue) value = maxValue;

    // Normalize value to a 0-1 range
    float ratio = (value - minValue) / (maxValue - minValue);

    // Extract RGBA components of the start and end colors
    unsigned int rStart = (colorStart >> 24) & 0xFF;
    unsigned int gStart = (colorStart >> 16) & 0xFF;
    unsigned int bStart = (colorStart >> 8) & 0xFF;
    unsigned int aStart = colorStart & 0xFF;

    unsigned int rEnd = (colorEnd >> 24) & 0xFF;
    unsigned int gEnd = (colorEnd >> 16) & 0xFF;
    unsigned int bEnd = (colorEnd >> 8) & 0xFF;
    unsigned int aEnd = colorEnd & 0xFF;

    // Interpolate each component
    unsigned int r = (unsigned int)((rEnd - rStart) * ratio + rStart);
    unsigned int g = (unsigned int)((gEnd - gStart) * ratio + gStart);
    unsigned int b = (unsigned int)((bEnd - bStart) * ratio + bStart);
    unsigned int a = (unsigned int)((aEnd - aStart) * ratio + aStart);

    // Recombine RGBA components into a single color value
    return (r << 24) | (g << 16) | (b << 8) | a;
}







void applyInitialPerturbation(Electron *electron, float initialVx, float initialVy) {
    electron->vx = initialVx;
    electron->vy = initialVy;
}


// Function to apply a Gaussian perturbation to the fields
void applyPerturbation(Voidness* grid, int centerX, int centerY, float strength, float width) {
    float sigma = width / 2.0f;  // Width of the Gaussian perturbation

    // Loop over the grid
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            // Calculate the distance from the perturbation center
            float distance = sqrtf((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY));

            // Calculate the Gaussian function value
            float gaussian = expf(-0.5f * (distance * distance) / (sigma * sigma));

            // Apply the perturbation to the fields
            grid[y * GRID_WIDTH + x].Ex += strength * gaussian;
            grid[y * GRID_WIDTH + x].Ey += strength * gaussian;
            grid[y * GRID_WIDTH + x].Bz += strength * gaussian;
        }
    }
}
