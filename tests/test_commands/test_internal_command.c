#include "commands.h"
#include "unity.h"

void setUp(void) {
    // Set up code if needed
}

void tearDown(void) {
    // Tear down code if needed
}

// Function prototypes
void test_is_internal_command(void);

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_is_internal_command);
    return UNITY_END();
}

void test_is_internal_command(void) {
    char* args1[] = {"cd", NULL};
    char* args2[] = {"invalid", NULL};

    TEST_ASSERT_TRUE(is_internal_command(args1));
    TEST_ASSERT_FALSE(is_internal_command(args2));
}
