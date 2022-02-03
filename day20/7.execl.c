#include <func.h>

int main(int argc, char const *argv[]) {
    execl("./add", "argv1", "3", "4", NULL);
    return 0;
}
