#include "hash.h"
#include "global.h"
#include "ui.h"

unsigned int hash(char key) { return key % HASHMAP_SIZE; }

hashmap *hashmap_create() { return NULL; }

uint8_t hashmap_check(hashmap *map, char character) {

  unsigned char key = (unsigned char)character;

  return (map->bitfield[key / 8] & (1 << (key % 8))) != 0;
}

SDL_Texture *hashmap_get(hashmap *map, char character) {

  unsigned char key = (unsigned char)character;

  return NULL;
}

void hashmap_add(hashmap *map, char key, graphics *gfx, TTF_Font *font,
                 SDL_Colour *colour) {}
