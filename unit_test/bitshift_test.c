#include "../bitshift_utils.h"
#include <assert.h>
#include <stdio.h>

void test_s32StoreValues(void) {
    s32 result = s32StoreValues(0x12, 0x34, 0x56, 0x78);
    assert(result == 0x12345678);
}

void test_s32ValueGet(void) {
    s32 container = 0x12345678;
    assert(s32ValueGet(container, BYTE1_SHIFT) == 0x12);
    assert(s32ValueGet(container, BYTE2_SHIFT) == 0x34);
    assert(s32ValueGet(container, BYTE3_SHIFT) == 0x56);
    assert(s32ValueGet(container, BYTE4_SHIFT) == 0x78);
}

void test_s32Value1Get(void) {
    s32 container = 0x12345678;
    assert(s32Value1Get(container) == 0x12);
	assert(s32Value1Get(container) == s32ValueGet(container, BYTE1_SHIFT));
}

void test_s32Value2Get(void) {
    s32 container = 0x12345678;
    assert(s32Value2Get(container) == 0x34);
	assert(s32Value2Get(container) == s32ValueGet(container, BYTE2_SHIFT));
}

void test_s32Value3Get(void) {
    s32 container = 0x12345678;
    assert(s32Value3Get(container) == 0x56);
	assert(s32Value3Get(container) == s32ValueGet(container, BYTE3_SHIFT));
}

void test_s32Value4Get(void) {
    s32 container = 0x12345678;
    assert(s32Value4Get(container) == 0x78);
	assert(s32Value4Get(container) == s32ValueGet(container, BYTE4_SHIFT));
}

int main(void) {
    // Run tests
    test_s32StoreValues();
    test_s32ValueGet();
    test_s32Value1Get();
    test_s32Value2Get();
    test_s32Value3Get();
    test_s32Value4Get();

    printf("All bitshift_utils tests passed.\n");
    return 0;
}
