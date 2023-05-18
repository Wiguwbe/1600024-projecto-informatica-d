#include <stdlib.h>
#include <check.h>
#include "allocator.h"

START_TEST(test_allocator_create) {
    size_t block_size = 10;
    size_t num_blocks = 5;

    Allocator* allocator = allocator_create(block_size, num_blocks);

    ck_assert_ptr_nonnull(allocator);
    ck_assert_ptr_nonnull(allocator->ptr);
    ck_assert_int_eq(allocator->block_size, block_size);
    ck_assert_int_eq(allocator->num_blocks, num_blocks);
    ck_assert_int_eq(allocator->used_blocks, 0);

    allocator_destroy(allocator);
}

START_TEST(test_allocator_alloc) {
    size_t block_size = 10;
    size_t num_blocks = 5;

    Allocator* allocator = allocator_create(block_size, num_blocks);

    void* ptr = allocator_alloc(allocator);

    ck_assert_ptr_nonnull(ptr);
    ck_assert_int_eq(allocator->used_blocks, 1);

    allocator_destroy(allocator);
}

START_TEST(test_allocator_realloc) {
    size_t block_size = 10;
    size_t num_blocks = 5;

    Allocator* allocator = allocator_create(block_size, num_blocks);

    void* ptr = allocator_alloc(allocator);
    ck_assert_ptr_nonnull(ptr);

    void* new_ptr = allocator_realloc(allocator, ptr);
    ck_assert_ptr_eq(new_ptr, allocator->ptr);

    allocator_destroy(allocator);
}

START_TEST(test_allocator_free) {
    size_t block_size = 10;
    size_t num_blocks = 5;

    Allocator* allocator = allocator_create(block_size, num_blocks);

    void* ptr = allocator_alloc(allocator);
    ck_assert_ptr_nonnull(ptr);

    allocator_free(allocator, ptr);
    ck_assert_int_eq(allocator->used_blocks, 0);

    allocator_destroy(allocator);
}

Suite* allocator_suite() {
    Suite* suite = suite_create("Allocator");

    TCase* tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_allocator_create);
    tcase_add_test(tc_core, test_allocator_alloc);
    tcase_add_test(tc_core, test_allocator_realloc);
    tcase_add_test(tc_core, test_allocator_free);
    suite_add_tcase(suite, tc_core);

    return suite;
}

int main() {
    Suite* suite = allocator_suite();
    SRunner* runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
