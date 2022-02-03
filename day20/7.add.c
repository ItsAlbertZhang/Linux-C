#include <func.h>

int main(int argc, char const *argv[]) {
    ARGS_CHECK(argc, 3)
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    printf("Ths sum is %d.\n", a + b);
    return 0;
}
