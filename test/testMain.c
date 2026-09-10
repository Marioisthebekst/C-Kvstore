#include <stdio.h>
#include "test.h"

int main(void) {
    testCreateFree();
    testInsertGet();
    testDelete();
    testResizeShrink();
    testTtlRelative();
    testTtlAbsolute();
    testIncrDecr();
    testLruEviction();
    testReplIntegration();
    testSaveLoad();

    printf("\n>>> ALL TESTS PASSED! <<<\n");
    return 0;
}