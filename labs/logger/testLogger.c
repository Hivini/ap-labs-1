#include "logger.h"

int main() {
    errorf("Error testing %d", 3);
    infof("Message %d", 1);
    warnf("Hello %d %s", 2, "cc");
    panicf("Kernel error? %d", 4);
    return 0;
}