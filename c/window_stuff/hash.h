#ifndef HASH_H
#define HASH_H

#include "global.h"
#include "ui.h"

#define HASHMAP_SIZE 256
#define HASH_COL 16
#define HASH_ROW 16

typedef struct {
  uint8_t bitfield[32];
  SDL_Texture *hashmap;
} hashmap;

unsigned int hash(char key);

hashmap *create_hashmap();

uint8_t hashmap_check(hashmap *map, char key);

SDL_Texture *hashmap_get(hashmap *map, char key);

void hashmap_add(hashmap *map, char key, graphics *gfx, TTF_Font *font,
                 SDL_Colour *colour);

#endif
