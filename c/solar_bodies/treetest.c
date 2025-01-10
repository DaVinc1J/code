#include <stdio.h>
#include <stdlib.h>

typedef struct {
  double x;
  double y;
} vector;

typedef struct object {
  int mass;
  vector position;
  vector velocity;
  int id;
  int parentId;
  struct object** children;
  int numChildren;
} object;

object* createObject(int id, int parentId, int mass, double posX, double posY, double velX, double velY) {
  object* obj = (object*)malloc(sizeof(object));
    
  obj->id = id;
  obj->parentId = parentId;
  obj->mass = mass;
  obj->numChildren = 0;
  obj->children = NULL;

  obj->position.x = posX;
  obj->position.y = posY;
  obj->velocity.x = velX;
  obj->velocity.y = velY;

  return obj;
}

void freeObject(object* obj) {
    if (obj != NULL) {
        
        printf("Starting to free Object ID: %d\n", obj->id);

        
        if (obj->children != NULL) {
            for (int i = 0; i < obj->numChildren; i++) {
                if (obj->children[i] != NULL) {
                    
                    printf("Freeing Child ID: %d under Parent ID: %d\n", obj->children[i]->id, obj->id);

                    freeObject(obj->children[i]);

                    obj->children[i] = NULL;
                }
            }
            
            printf("Freeing children array of Object ID: %d\n", obj->id);
            free(obj->children);
            obj->children = NULL;
        }

        printf("Freeing Object ID: %d\n", obj->id);
        free(obj);
    }
}

void addChild(object* parent, object* child) {
  parent->numChildren++;
  parent->children = (object**)realloc(parent->children, parent->numChildren * sizeof(object*));
  parent->children[parent->numChildren - 1] = child;
}

int compareByParentId(const void *a, const void *b) {
  object *objA = *(object **)a;
  object *objB = *(object **)b;
  return (objA->parentId - objB->parentId);
}

void buildRelationships(object** objs, int numObjs) {
  
  qsort(objs, numObjs, sizeof(object*), compareByParentId);

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

int main () {
  object* sun     = createObject(1, 0, 1000);
  object* earth   = createObject(2, 1, 100);
  object* mars    = createObject(3, 1, 100);
  object* moon    = createObject(4, 2, 10);
  object* phobos  = createObject(5, 3, 10);

  object* objs[] = {phobos, earth, sun, mars, moon};

  int numObjs = sizeof(objs) / sizeof(objs[0]);

  printf("Number of Objects: %d\n", numObjs);

  buildRelationships(objs, numObjs);

  for (int i = 0; i < numObjs; i++) {
    printf("Object ID: %d has %d children\n", objs[i]->id, objs[i]->numChildren);
    for (int j = 0; j < objs[i]->numChildren; j++) {
      printf("ID: %d, ", objs[i]->children[j]->id);
    }
    printf("\n");
  }

  for (int i = 0; i < numObjs; i++) {
    printf("Object Mass: %d\n", objs[i]->mass);
  }
  
  freeObject(objs[0]);

  for (int i = 0; i < numObjs; i++) {
    printf("Object Mass: %d\n", objs[i]->mass);
  }

  

  return 0;
  
}
