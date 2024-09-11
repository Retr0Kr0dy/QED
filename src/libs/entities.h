extern int GRID_WIDTH;
extern int GRID_HEIGHT;

struct electron {
    float posX;
    float posY;
    float velX;
    float velY;
};

struct wave {
    float posX;
    float posY;
    float velX;
    float velY;
};


struct voidness {
    float potential;
    float surrounding;
};

// Declare the array of type 'struct voidness'
struct voidness voidness_list[256 * 240];



struct electron electron_list[1024];
int electron_count = 0;

void addElectron(float posX, float posY, float velX, float velY) {
    struct electron elec;

    elec.posX = posX;
    elec.posY = posY;
    elec.velX = velX;
    elec.velY = velY;
    electron_list[electron_count] = elec;
    electron_count++;
}



