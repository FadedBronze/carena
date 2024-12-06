#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

typedef struct ArenaAllocator {
  uint32_t offset;
  uint32_t capacity;
  void* head;
} ArenaAllocator;

void arena_init(ArenaAllocator* arena, uint32_t initial_capacity_bytes) {
  arena->capacity = initial_capacity_bytes;
  arena->head = malloc(initial_capacity_bytes);
  arena->offset = 0;
}

void arena_double_capacity(ArenaAllocator* arena) {
  void* new_head = malloc(arena->capacity);
  memcpy(new_head, arena->head, arena->offset);
  free(arena->head);
  arena->head = new_head; 
}

void arena_destroy(ArenaAllocator* arena, uint32_t initial_capacity_bytes) {
  free(arena->head);
  memset(arena, 0, sizeof(ArenaAllocator));
}

void arena_clear(ArenaAllocator* arena) {
  arena->offset = 0;
}

void* arena_alloc_bytes(ArenaAllocator* arena, uint32_t bytes) {
  assert(arena->head != NULL);

  if (arena->offset + bytes > arena->capacity) {
    arena_double_capacity(arena);
  }

  void* location = arena->head + arena->offset;
  arena->offset += bytes;
  return location;
}

void* arena_push(ArenaAllocator* arena, void* data, uint32_t bytes) {
  char* pointer = arena_alloc_bytes(arena, bytes);
  memcpy(pointer, data, bytes);
  return pointer;
}

struct Foo {
  int bar;
  long long baz;
};

int main() {
  ArenaAllocator arena;
  arena_init(&arena, 24);

  uint32_t number = 40;
  void* data = arena_push(&arena, &number, sizeof(uint32_t));
  number = 20;
  void* data1 = arena_push(&arena, &number, sizeof(uint32_t));
  struct Foo foo;
  foo.bar = 20;
  foo.baz = 100000;
  void* data2 = arena_push(&arena, &foo, sizeof(struct Foo));
  number = 45;
  void* data3 = arena_push(&arena, &number, sizeof(uint32_t));

  printf("%llu", ((struct Foo*)data2)->baz);

  return 0;
}
