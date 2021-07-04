#include <stdlib.h>
#include <malloc.h>

extern void *__libc_malloc(size_t size);
extern void __libc_free(void *ptr);

void *malloc(size_t size)
{
    void *result = __libc_malloc(size);
    fprintf(stderr, "malloc(%d) = %p\n", (int)size, result);
    return result;
}

void free(void *ptr)
{
    __libc_free(ptr);
    fprintf(stderr, "free(%p)\n", ptr);
}

int main(){
  void* ptr = malloc(100);
  free(ptr);


}
