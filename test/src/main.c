#include "main.h"
#include "lib_a.h"
#include "obj_a.h"

int main() {
    printf("Sent from main\n");
    obj_a();
    obj_b();
    lib_a();
    lib_b();
}