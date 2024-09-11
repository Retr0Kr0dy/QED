extern int GRID_WIDTH;
extern int GRID_HEIGHT;

struct Vector3 {
    float x;
    float y;
};

struct ElectricField {
    struct Vector3 E; // Electric field vector
};

struct MagneticField {
    struct Vector3 B; // Magnetic field vector
};

struct voidness {
    float potential;
    float surrounding;
    struct ElectricField eField;
    struct MagneticField bField;
};

// Declare the array of type 'struct voidness'
struct voidness voidness_list[256 * 240];


