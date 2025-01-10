#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct object {
    int colour[4];
    double pos[2];
    double vel[2];
    int mass;
    int id;
    int parentId;
    struct object** children;
    int numChildren;
} object;

typedef struct {
    int quit;
    int showSpecs;
    int menu;
} options;

typedef struct {
    TTF_Font* font;
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Event event;
} graphics;

object* createObject(int colour[4], double pos[2], double vel[2], int mass, int id, int parentId) {
    object* obj = (object*)malloc(sizeof(object));

    obj->mass = mass;
    obj->id = id;
    obj->parentId = parentId;
    obj->numChildren = 0;
    obj->children = NULL;

    for (int i = 0; i < 4; i++) {
        obj->colour[i] = colour[i];
    }

    for (int i = 0; i < 2; i++) {
        obj->pos[i] = pos[i];
        obj->vel[i] = vel[i];
    }

    return obj;
}

void freeObject(object* obj) {
    if (obj->children != NULL) {
        for (int i = 0; i < obj->numChildren; i++) {
            freeObject(obj->children[i]);
        }
        free(obj->children);
    }
    free(obj);
}

void addChild(object* parent, object* child) {
    parent->numChildren++;
    parent->children = (object**)realloc(parent->children, parent->numChildren * sizeof(object*));
    parent->children[parent->numChildren - 1] = child;
}

void buildRelationships(object** objs, int numObjs) {
    for (int i = 0; i < numObjs; i++) {
        if (objs[i]->parentId != 0) {
            for (int j = 0; j < numObjs; j++) {
                if (objs[j]->id == objs[i]->parentId) {
                    addChild(objs[j], objs[i]);
                    break;
                }
            }
        }
    }
}

void calculateGravitationalForces(object** objs, int numObjs) {
    double G = 5.0e-2;  // Gravitational constant

    for (int i = 0; i < numObjs; i++) {
        for (int j = 0; j < numObjs; j++) {
            if (i != j) {
                object* obj1 = objs[i];
                object* obj2 = objs[j];

                double dx = obj2->pos[0] - obj1->pos[0];
                double dy = obj2->pos[1] - obj1->pos[1];
                double distanceSquared = dx * dx + dy * dy;
                double distance = sqrt(distanceSquared);
                double force = (G * obj1->mass * obj2->mass) / distanceSquared;

                double ax = force * dx / distance / obj1->mass;
                double ay = force * dy / distance / obj1->mass;

                obj1->vel[0] += ax;
                obj1->vel[1] += ay;
            }
        }
    }
}

void updatePositions(object** objs, int numObjs) {
    for (int i = 0; i < numObjs; i++) {
        object* obj = objs[i];
        obj->pos[0] += obj->vel[0];
        obj->pos[1] += obj->vel[1];
    }
}

void initialise(graphics* gfx, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    TTF_Init();

    gfx->font = TTF_OpenFont("VictorMonoNerdFont.ttf", 14);
    gfx->win = SDL_CreateWindow(
        "...",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0x00000004
    );
    gfx->ren = SDL_CreateRenderer(gfx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void checkEvents(graphics* gfx, options* option) {
    while (SDL_PollEvent(&gfx->event)) {
        if (gfx->event.type == SDL_QUIT) {
            option->quit = 1;
        }
        if (gfx->event.type == SDL_KEYDOWN) {
            switch (gfx->event.key.keysym.sym) {
            case SDLK_ESCAPE:
                option->menu = 1;
                break;
            case SDLK_SPACE:
                option->showSpecs = 1;
                break;
            default:
                break;
            }
        }
        if (gfx->event.type == SDL_MOUSEBUTTONDOWN) {
            if (gfx->event.button.button == SDL_BUTTON_LEFT) {
                // add stuff here
            }
        }
    }
}

void exitSDL(graphics* gfx) {
    TTF_CloseFont(gfx->font);
    SDL_DestroyRenderer(gfx->ren);
    SDL_DestroyWindow(gfx->win);
    TTF_Quit();
    SDL_Quit();
}

void renderObjects(graphics* gfx, object** objs, int numObjs) {
    SDL_SetRenderDrawColor(gfx->ren, 0, 0, 0, 255);  // Clear with black
    SDL_RenderClear(gfx->ren);

    for (int i = 0; i < numObjs; i++) {
        SDL_SetRenderDrawColor(gfx->ren, objs[i]->colour[0], objs[i]->colour[1], objs[i]->colour[2], objs[i]->colour[3]);
        SDL_Rect rect = { (int)objs[i]->pos[0], (int)objs[i]->pos[1], 10, 10 };
        SDL_RenderFillRect(gfx->ren, &rect);
    }

    SDL_RenderPresent(gfx->ren);
}

int main() {
    // SDL Initialising
    const int width = 800;
    const int height = 600;
    graphics gfx;
    initialise(&gfx, width, height);
    SDL_SetRenderDrawColor(gfx.ren, 0, 0, 0, 255);

    // Object Creation
    object* sun = createObject((int[4]){255, 255, 0, 255}, (double[2]){400, 300}, (double[2]){0, 0}, 10000, 1, 0);
    object* earth = createObject((int[4]){0, 0, 163, 255}, (double[2]){600, 300}, (double[2]){0, 2}, 6, 2, 1);
    object* moon = createObject((int[4]){189, 195, 199, 255}, (double[2]){650, 300}, (double[2]){0, 1}, 1, 3, 2);

    object* objs[] = { sun, earth, moon };
    const int numObjs = sizeof(objs) / sizeof(objs[0]);

    // Build hierarchy automatically based on parentId
    buildRelationships(objs, numObjs);

    // options stuff
    options option = { .quit = 0, .showSpecs = 0, .menu = 0 };

    while (!option.quit) {
        checkEvents(&gfx, &option);

        // Apply gravitational forces between all pairs and update positions
        calculateGravitationalForces(objs, numObjs);
        updatePositions(objs, numObjs);

        renderObjects(&gfx, objs, numObjs);
    }

    // Free memory
    freeObject(objs[0]);
    

    exitSDL(&gfx);
    return 0;
}
