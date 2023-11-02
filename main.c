#include <stdio.h>
#include <unistd.h>
#include "qff.h"

void fiber3(void);

void fiber1(void) {
    printf("🧶    fiber 1 starts!\n");
    
    for (int i = 0; i < 7; i++) {
        printf("🏃    fiber 1 - i is %d\n", i);
        usleep(500000);
        qff_yield();
        printf("🤝    --> yielded back to 1\n");
    }

    printf("✅    fiber 1 is done 🎉\n");
}

void fiber2(void) {
    printf("🧶    fiber 2 starts!\n");

    for (int i = 0; i < 10; i++) {
        printf("🏃    fiber 2 - i is %d\n", i);

        if (i == 5) {
            printf("📆    Scheduling fiber 3\n");
            qff_schedule(fiber3);
        }

        usleep(500000);
        qff_yield();
        printf("🤝    --> yielded back to 2\n");
    }

    printf("✅    fiber 2 is done 🎉\n");
}

void fiber3(void) {
    printf("🧶    fiber 3 starts!\n");
    
    for (int i = 0; i < 4; i++) {
        printf("🏃    fiber 3 - i is %d\n", i);
        usleep(500000);
        qff_yield();
        printf("🤝    --> yielded back to 3\n");
    }

    printf("✅    fiber 3 is done 🎉\n");
}

void init(void) {
    qff_schedule(fiber1);
    qff_schedule(fiber2);    
}

int main(void) {
    printf("➡️     Starting fibers\n");

    qff_run(init);

    printf("⬅️     All finished!\n");
}
