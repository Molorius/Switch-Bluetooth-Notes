

#include <stdio.h>

#include "register.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Error: needs to have controller name as input\n");
        return 1;
    }
    printf("%s\n", argv[1]);
    register_name(argv[1]);
    register_records(argv[1]);
    return 0;
}