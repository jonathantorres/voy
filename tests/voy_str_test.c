#include "voy_str.h"
#include "voy_unittest.h"
#include <string.h>

void voy_array_of_voy_strs_cb(void *str)
{
    voy_str_t *s = (voy_str_t *)str;
    voy_str_free(s);
}

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
    char *string   = voy_str_get_string(str);
    voy_assert(string != NULL, "The string should not be NULL");
    voy_assert(strlen(string) == (size_t)str->len,
               "The lengths of str and the string should be equal");
    voy_str_free(str);
    return NULL;
}

char *test_str_concat()
{
    voy_str_t *str = voy_str_new("Jonathan");
    str            = voy_str_concat(str, " Torres");
    voy_assert(str != NULL, "The string should not be NULL");
    voy_assert(str->len == 15, "The len of the string should be 15");
    voy_assert(strcmp(str->string, "Jonathan Torres") == 0, "The strings should be equal");
    voy_str_free(str);
    return NULL;
}

char *test_concat_str()
{
    voy_str_t *str       = voy_str_new("Jonathan");
    voy_str_t *new_chars = voy_str_new(" Torres");
    str                  = voy_str_concat_voy_str(str, new_chars);
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
    bool found     = voy_str_contains(str, "ona");
    voy_assert(found == true, "Found should be true");
    found = voy_str_contains(str, "ola");
    voy_assert(found == false, "Found should be false");
    voy_str_free(str);
    return NULL;
}

char *test_str_contains_char()
{
    voy_str_t *str = voy_str_new("Jonathan");
    bool found     = voy_str_contains_char(str, 'J');
    voy_assert(found == true, "The character 'J' is in 'Jonathan'");
    found = voy_str_contains_char(str, 'i');
    voy_assert(found == false, "The character 'i' is not in 'Jonathan'");
    voy_str_free(str);
    return NULL;
}

char *test_contains_str()
{
    voy_str_t *str1 = voy_str_new("Jonathan");
    voy_str_t *str2 = voy_str_new("ona");
    voy_str_t *str3 = voy_str_new("ola");
    bool found      = voy_str_contains_voy_str(str1, str2);
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
    bool haspre    = voy_str_has_prefix(str, "Jon");
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
    bool haspre     = voy_str_has_prefix_voy_str(str1, str2);
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
    bool hassu     = voy_str_has_suffix(str, "han");
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
    bool hassu      = voy_str_has_suffix_voy_str(str1, str2);
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

char *test_str_dup()
{
    char *me  = "Jonathan Torres";
    char *new = voy_str_dup(me);
    voy_assert(new != NULL, "The new string should not be NULL");
    voy_assert(strcmp(new, me) == 0, "The new string should be equal to the old one");
    free(new);
    return NULL;
}

char *test_str_dup_str()
{
    voy_str_t *me  = voy_str_new("Jonathan Torres");
    voy_str_t *new = voy_str_dup_str(me);
    voy_assert(new != NULL, "The new string should not be NULL");
    voy_assert(strcmp(new->string, me->string) == 0,
               "The new string should be equal to the old one");
    voy_str_free(me);
    voy_str_free(new);
    return NULL;
}

char *test_str_split_by_char()
{
    voy_str_t *s     = voy_str_new("one,two,three");
    voy_array_t *res = voy_str_split_by_char(s, ',');
    voy_str_t *cur_s = NULL;
    voy_assert(res->len == 3, "The length of the resulting array should be 3");
    cur_s = voy_array_get(res, 0);
    voy_assert(cur_s != NULL, "The string returned at index 0 should not be NULL");
    voy_assert(strcmp("one", cur_s->string) == 0, "The string at index 0 is not equal to \"one\"");

    cur_s = voy_array_get(res, 1);
    voy_assert(cur_s != NULL, "The string returned at index 1 should not be NULL");
    voy_assert(strcmp("two", cur_s->string) == 0, "The string at index 1 is not equal to \"two\"");

    cur_s = voy_array_get(res, 2);
    voy_assert(cur_s != NULL, "The string returned at index 2 should not be NULL");
    voy_assert(strcmp("three", cur_s->string) == 0,
               "The string at index 2 is not equal to \"three\"");

    voy_str_free(s);
    voy_array_free(res, voy_array_of_voy_strs_cb);
    return NULL;
}

char *test_str_split_by_char_no_delim()
{
    voy_str_t *s     = voy_str_new("one");
    voy_array_t *res = voy_str_split_by_char(s, ',');
    voy_str_t *cur_s = NULL;
    voy_assert(res != NULL, "The returned array cannot be NULL");
    voy_assert(res->len == 1, "The length of the resulting array should be 1");
    cur_s = voy_array_get(res, 0);
    voy_assert(cur_s != NULL, "The string returned at index 0 should not be NULL");
    voy_assert(strcmp("one", cur_s->string) == 0, "The string at index 0 is not equal to \"one\"");

    voy_str_free(s);
    voy_array_free(res, voy_array_of_voy_strs_cb);
    return NULL;
}

char *test_str_split_by_char_with_equals()
{
    voy_str_t *s     = voy_str_new("name = localhost");
    voy_array_t *res = voy_str_split_by_char(s, '=');
    voy_str_t *cur_s = NULL;
    voy_assert(res->len == 2, "The length of the resulting array should be 2");
    cur_s = voy_array_get(res, 0);
    voy_assert(cur_s != NULL, "The string returned at index 0 should not be NULL");
    voy_assert(strcmp("name ", cur_s->string) == 0,
               "The string at index 0 is not equal to \"name \"");

    cur_s = voy_array_get(res, 1);
    voy_assert(cur_s != NULL, "The string returned at index 1 should not be NULL");
    voy_assert(strcmp(" localhost", cur_s->string) == 0,
               "The string at index 1 is not equal to \" localhost\"");

    voy_str_free(s);
    voy_array_free(res, voy_array_of_voy_strs_cb);
    return NULL;
}

char *test_str_trim()
{
    voy_str_t *s1 = voy_str_new("   Jonathan");
    voy_str_t *s2 = voy_str_new("  Jonathan   ");
    voy_str_t *s3 = voy_str_new("Jonathan   ");
    voy_str_trim(s1);
    voy_str_trim(s2);
    voy_str_trim(s3);
    voy_assert(strcmp(s1->string, "Jonathan") == 0,
               "The resulting string from (s1) does not have the expected value");
    voy_assert(strcmp(s2->string, "Jonathan") == 0,
               "The resulting string from (s2) does not have the expected value");
    voy_assert(strcmp(s3->string, "Jonathan") == 0,
               "The resulting string from (s3) does not have the expected value");

    voy_str_free(s1);
    voy_str_free(s2);
    voy_str_free(s3);
    return NULL;
}

char *test_str_equals()
{
    voy_str_t *s1 = voy_str_new("one");
    voy_str_t *s2 = voy_str_new("");

    bool res;
    res = voy_str_equals(s1, "one");
    voy_assert(res == true, "The string in s1 should be equal to 'one'");
    res = voy_str_equals(s1, "two");
    voy_assert(res == false, "The string in s1 should not be equal to 'two'");
    res = voy_str_equals(s2, "one");
    voy_assert(res == false, "The string in s2 should not be equal to 'one'");
    res = voy_str_equals(s2, "");
    voy_assert(res == true, "The strings should be equal since they are empty");

    voy_str_free(s1);
    voy_str_free(s2);
    return NULL;
}

char *test_str_equals_str()
{
    voy_str_t *s1 = voy_str_new("one");
    voy_str_t *s2 = voy_str_new("");
    voy_str_t *s3 = voy_str_new("one");
    voy_str_t *s4 = voy_str_new("two");
    voy_str_t *s5 = voy_str_new("");

    bool res;
    res = voy_str_equals_voy_str(s1, s3);
    voy_assert(res == true, "The string in s1 should be equal to the one in s3");
    res = voy_str_equals_voy_str(s1, s4);
    voy_assert(res == false, "The string in s1 should not be equal to the one in s4");
    res = voy_str_equals_voy_str(s2, s1);
    voy_assert(res == false, "The string in s2 should not be equal to the one in s1");
    res = voy_str_equals_voy_str(s2, s5);
    voy_assert(res == true, "The strings should be equal since they are empty");

    voy_str_free(s1);
    voy_str_free(s2);
    voy_str_free(s3);
    voy_str_free(s4);
    voy_str_free(s5);
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
    voy_run_test(test_str_contains_char);
    voy_run_test(test_contains_str);
    voy_run_test(test_str_has_prefix);
    voy_run_test(test_str_has_prefix_str);
    voy_run_test(test_str_suffix);
    voy_run_test(test_str_suffix_str);
    voy_run_test(test_str_dup);
    voy_run_test(test_str_dup_str);
    voy_run_test(test_str_split_by_char);
    voy_run_test(test_str_split_by_char_no_delim);
    voy_run_test(test_str_split_by_char_with_equals);
    voy_run_test(test_str_trim);
    voy_run_test(test_str_equals);
    voy_run_test(test_str_equals_str);
    voy_end_tests();
    return 0;
}
