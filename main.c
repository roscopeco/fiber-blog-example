#include <stdio.h>
#include <unistd.h>
#include "qff.h"

void task1(void) {
    for (int i = 0; i < 3; i++) {
        printf("Task 1 - i is %d\n", i);
        usleep(500);
        qff_yield();
        printf("  --> yielded back to 1\n");
    }

    printf("Task 1 is done\n");
}

void task2(void) {
    for (int i = 0; i < 5; i++) {
        printf("Task 2 - i is %d\n", i);
        usleep(500);
        qff_yield();
        printf("  --> yielded back to 2\n");
    }

    printf("Task 2 is done\n");
}

void init(void) {
    qff_schedule(task1);
    qff_schedule(task2);    
}

int main(void) {
    qff_start(init);
}
