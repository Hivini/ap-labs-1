#include "logger.h"

int main() {
    infof("Message %d", 1);
    warnf("Hello %d %s", 2, "cc");
    errorf("Error testing %d", 3);
    panicf("Kernel error? %d", 4);
    return 0;
}