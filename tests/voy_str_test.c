#include <string.h>
#include "voy_str.h"
#include "voy_unittest.h"

char *test_create_empty_string()
{
    voy_str_t *str = voy_str_new("");
    voy_assert(str != NULL, "str should not be NULL");
    voy_assert(str->len == 0, "The len of str should be 0");
    voy_assert(str->string != NULL, "The string should not be NULL");
    voy_assert(strcmp(str->string, "") == 0, "The string should be empty");
    voy_str_free(str);
    return NULL;
}

char *test_create_string()
{
    voy_str_t *str = voy_str_new("Jonathan Torres");
    voy_assert(str->len == 15, "The length of str should be 15");
    voy_assert(strcmp(str->string, "Jonathan Torres") == 0, "The strings do not match");
    voy_str_free(str);
    return NULL;
}

char *test_free()
{
    voy_str_t *str = voy_str_new("");
    voy_str_free(str);
    return NULL;
}

char *test_len()
{
    voy_str_t *str = voy_str_new("Jonathan");
    voy_assert(str->len == 8, "The len of the string should be 8");
    voy_str_free(str);
    return NULL;
}

char *test_get_string()
{
    voy_str_t *str = voy_str_new("Jonathan");
    char *string = voy_str_get_string(str);
    voy_assert(string != NULL, "The string should not be NULL");
    voy_assert(strlen(string) == (unsigned long)str->len, "The lengths of str and the string should be equal");
    voy_str_free(str);
    return NULL;
}

char *test_str_concat()
{
    voy_str_t *str = voy_str_new("Jonathan");
    str = voy_str_concat(str, " Torres");
    voy_assert(str != NULL, "The string should not be NULL");
    voy_assert(str->len == 15, "The len of the string should be 15");
    voy_assert(strcmp(str->string, "Jonathan Torres") == 0, "The strings should be equal");
    voy_str_free(str);
    return NULL;
}

char *test_concat_str()
{
    voy_str_t *str = voy_str_new("Jonathan");
    voy_str_t *new_chars = voy_str_new(" Torres");
    str = voy_str_concat_voy_str(str, new_chars);
    voy_assert(str != NULL, "The string should not be NULL");
    voy_assert(str->len == 15, "The len of the string should be 15");
    voy_assert(strcmp(str->string, "Jonathan Torres") == 0, "The strings should be equal");
    voy_str_free(str);
    voy_str_free(new_chars);
    return NULL;
}

char *test_str_contains()
{
    voy_str_t *str = voy_str_new("Jonathan");
    bool found = voy_str_contains(str, "ona");
    voy_assert(found == true, "Found should be true");
    found = voy_str_contains(str, "ola");
    voy_assert(found == false, "Found should be false");
    voy_str_free(str);
    return NULL;
}

char *test_contains_str()
{
    voy_str_t *str1 = voy_str_new("Jonathan");
    voy_str_t *str2 = voy_str_new("ona");
    voy_str_t *str3 = voy_str_new("ola");
    bool found = voy_str_contains_voy_str(str1, str2);
    voy_assert(found == true, "Found should be true");
    found = voy_str_contains_voy_str(str1, str3);
    voy_assert(found == false, "Found should be false");
    voy_str_free(str1);
    voy_str_free(str2);
    voy_str_free(str3);
    return NULL;
}

char *test_str_has_prefix()
{
    voy_str_t *str = voy_str_new("Jonathan");
    bool haspre = voy_str_has_prefix(str, "Jon");
    voy_assert(haspre == true, "Has prefix should be true");
    haspre = voy_str_has_prefix(str, "lola");
    voy_assert(haspre == false, "The lola prefix should be false");
    haspre = voy_str_has_prefix(str, "nath");
    voy_assert(haspre == false, "The nath prefix should be false");
    voy_str_free(str);
    return NULL;
}

char *test_str_has_prefix_str()
{
    voy_str_t *str1 = voy_str_new("Jonathan");
    voy_str_t *str2 = voy_str_new("Jon");
    voy_str_t *str3 = voy_str_new("lola");
    voy_str_t *str4 = voy_str_new("nath");
    bool haspre = voy_str_has_prefix_voy_str(str1, str2);
    voy_assert(haspre == true, "Has prefix should be true");
    haspre = voy_str_has_prefix_voy_str(str1, str3);
    voy_assert(haspre == false, "The lola prefix should be false");
    haspre = voy_str_has_prefix_voy_str(str1, str4);
    voy_assert(haspre == false, "The nath prefix should be false");
    voy_str_free(str1);
    voy_str_free(str2);
    voy_str_free(str3);
    voy_str_free(str4);
    return NULL;
}

char *test_str_suffix()
{
    voy_str_t *str = voy_str_new("Jonathan");
    bool hassu = voy_str_has_suffix(str, "han");
    voy_assert(hassu == true, "The suffix han should be true");
    hassu = voy_str_has_suffix(str, "than");
    voy_assert(hassu == true, "The suffix than should be true");
    hassu = voy_str_has_suffix(str, "Jona");
    voy_assert(hassu == false, "The suffix Jona should be false");
    hassu = voy_str_has_suffix(str, "lola");
    voy_assert(hassu == false, "The suffix lola should be false");
    voy_str_free(str);
    return NULL;
}

char *test_str_suffix_str()
{
    voy_str_t *str1 = voy_str_new("Jonathan");
    voy_str_t *str2 = voy_str_new("han");
    voy_str_t *str3 = voy_str_new("than");
    voy_str_t *str4 = voy_str_new("Jona");
    voy_str_t *str5 = voy_str_new("lola");
    bool hassu = voy_str_has_suffix_voy_str(str1, str2);
    voy_assert(hassu == true, "The suffix han should be true");
    hassu = voy_str_has_suffix_voy_str(str1, str3);
    voy_assert(hassu == true, "The suffix than should be true");
    hassu = voy_str_has_suffix_voy_str(str1, str4);
    voy_assert(hassu == false, "The suffix Jona should be false");
    hassu = voy_str_has_suffix_voy_str(str1, str5);
    voy_assert(hassu == false, "The suffix lola should be false");
    voy_str_free(str1);
    voy_str_free(str2);
    voy_str_free(str3);
    voy_str_free(str4);
    voy_str_free(str5);
    return NULL;
}

int main(void)
{
    voy_start_tests("voy str tests");
    voy_run_test(test_create_empty_string);
    voy_run_test(test_create_string);
    voy_run_test(test_free);
    voy_run_test(test_len);
    voy_run_test(test_get_string);
    voy_run_test(test_str_concat);
    voy_run_test(test_concat_str);
    voy_run_test(test_str_contains);
    voy_run_test(test_contains_str);
    voy_run_test(test_str_has_prefix);
    voy_run_test(test_str_has_prefix_str);
    voy_run_test(test_str_suffix);
    voy_run_test(test_str_suffix_str);
    voy_end_tests();
    return 0;
}
