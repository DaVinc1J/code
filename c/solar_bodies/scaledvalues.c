
#include <stdio.h>
#include <math.h>

// Constants
#define G (4 * M_PI * M_PI)  // Gravitational constant in AU^3 / (Solar Mass * Year^2)
#define AU 1.0               // 1 AU (distance from Earth to Sun)
#define M_SUN 1.0            // 1 Solar mass
#define YEAR 1.0             // 1 year

// Structure to represent a celestial object
typedef struct {
    double mass;               // Mass in solar masses
    double positionX;          // Position in AU
    double positionY;          // Position in AU
    double velocityX;          // Velocity in AU/year
    double velocityY;          // Velocity in AU/year
    double gravitationalRadius; // Gravitational radius in AU
} Object;

// Function to calculate the distance between two objects
double calculateDistance(Object *obj1, Object *obj2) {
    double dx = obj2->positionX - obj1->positionX;
    double dy = obj2->positionY - obj1->positionY;
    return sqrt(dx * dx + dy * dy);
}

// Function to calculate gravitational force if within linking distance
int calculateGravitationalForce(Object *parent, Object *child, double *forceX, double *forceY) {
    double distance = calculateDistance(parent, child);

    // Only calculate force if within gravitationalRadius of parent
    if (distance > parent->gravitationalRadius) {
        *forceX = 0;
        *forceY = 0;
        return 0;  // Not linked
    }

    double dx = child->positionX - parent->positionX;
    double dy = child->positionY - parent->positionY;

    double forceMagnitude = G * parent->mass * child->mass / (distance * distance);
    *forceX = -forceMagnitude * dx / distance;
    *forceY = -forceMagnitude * dy / distance;

    return 1;  // Linked
}

// Function to update the position and velocity of an object based on the force applied
void updateObject(Object *obj, double forceX, double forceY, double dt) {
    // Calculate acceleration
    double ax = forceX / obj->mass;
    double ay = forceY / obj->mass;

    // Update velocity
    obj->velocityX += ax * dt;
    obj->velocityY += ay * dt;

    // Update position
    obj->positionX += obj->velocityX * dt;
    obj->positionY += obj->velocityY * dt;
}

int main() {
    // Example objects (Sun and Earth)
    Object sun = {
    .mass = M_SUN, 
    .positionX = 0.0, 
    .positionY = 0.0, 
    .velocityX = 0.0, 
    .velocityY = 0.0, 
    .gravitationalRadius = 10.0
  };  // Sun with gravitational radius of 10 AU
    Object earth = {
    .mass = 3.0e-6, 
    .positionX = AU, 
    .positionY = 0.0, 
    .velocityX = 0.0, 
    .velocityY = 2 * M_PI, 
    .gravitationalRadius = 1.0
  };  // Earth with radius 1 AU

    double dt = 0.01;  // Time step in years
    double forceX, forceY;
    int isLinked;

    // Simulate for 1 year (365 days)
    for (int i = 0; i < (int)(1.0 / dt); i++) {
        // Check and calculate gravitational force if within distance
        isLinked = calculateGravitationalForce(&sun, &earth, &forceX, &forceY);
        
        if (isLinked) {
            // Update Earth's position and velocity based on the gravitational force
            updateObject(&earth, forceX, forceY, dt);
        }

        // Print Earth's position
        printf("Day %d: Earth position: (%.3f, %.3f) AU\n", (int)(i * dt * 365), earth.positionX, earth.positionY);
    }

    return 0;
}
