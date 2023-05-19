#include <check.h>
#include <stdlib.h>
#include "allocator.h"

typedef struct {
    int id;
    char name[20];
} my_struct_t;

START_TEST(test_allocator_alloc)
{
    allocator_t* allocator = allocator_init(sizeof(my_struct_t));

    my_struct_t* struct1 = (my_struct_t*)allocator_alloc(allocator);
    struct1->id = 1;
    strncpy(struct1->name, "João", sizeof(struct1->name));

    my_struct_t* struct2 = (my_struct_t*)allocator_alloc(allocator);
    struct2->id = 2;
    strncpy(struct2->name, "Maria", sizeof(struct2->name));

    ck_assert_ptr_ne(struct1, struct2);
    ck_assert_int_eq(struct1->id, 1);
    ck_assert_str_eq(struct1->name, "João");
    ck_assert_int_eq(struct2->id, 2);
    ck_assert_str_eq(struct2->name, "Maria");

    allocator_free(allocator);
    free(allocator);
}
END_TEST

Suite* allocator_suite() {
    Suite* suite = suite_create("allocator_t");
    TCase* test_case = tcase_create("allocation");

    tcase_add_test(test_case, test_allocator_alloc);

    suite_add_tcase(suite, test_case);

    return suite;
}

int main() {
    Suite* suite = allocator_suite();
    SRunner* runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int num_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (num_failed == 0) ? 0 : 1;
}
