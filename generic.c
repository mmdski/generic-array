#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t nel;
  size_t width;
  char   values[1];
} _GArray;

typedef _GArray *GArray;

int
garray_new(GArray *a_ptr, size_t nel, size_t width) {

  GArray a;
  size_t sizeof_a = sizeof(*a) + width * nel - 1;
  a               = malloc(sizeof_a);
  a->nel          = nel;
  a->width        = width;
  *a_ptr          = a;

  return 0;
}

void
garray_free(GArray a) {
  free(a);
}

int
garray_set(GArray a, size_t i, void *value_ptr) {

  memcpy(&a->values + i * a->width, value_ptr, a->width);

  return 0;
}

int
garray_get(GArray a, size_t i, void *value_ptr) {

  memcpy(value_ptr, &a->values + i * a->width, a->width);

  return 0;
}

int
main(void) {

  size_t nel   = 20;
  size_t width = sizeof(double);
  GArray a     = NULL;
  if (garray_new(&a, nel, width)) {
    fputs("garray_new() failed\n", stderr);
    return EXIT_FAILURE;
  }

  double set_value;
  double get_value;
  for (size_t i = 0; i < nel; i++) {
    set_value = (double) i;
    garray_set(a, i, &set_value);
    garray_get(a, i, &get_value);
    printf("%f\n", get_value);
  }

  free(a);

  return EXIT_SUCCESS;
}
