#include <stdio.h>
#include <assert.h>

#include "../libft.h"

void test_fast_strcmp(void)
{
    assert(fast_strcmp("hello", "hello") == 0);

    assert(fast_strcmp("abc", "abcd") == ('\0' - 'd'));

    assert(fast_strcmp("abcd", "abc") == ('d' - '\0'));

    assert(fast_strcmp("foo", "bar") == ('f' - 'b'));

    assert(fast_strcmp("", "nonempty") == ('\0' - 'n'));
    assert(fast_strcmp("nonempty", "") == ('n' - '\0'));

    assert(fast_strcmp("", "") == 0);

	const char *long_str1 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    const char *long_str2 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    assert(fast_strcmp(long_str1, long_str2) == 0);

    const char *long_str3 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    const char *long_str4 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function. Extra characters.";
    assert(fast_strcmp(long_str3, long_str4) == ('\0' - ' '));

    const char *long_str5 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function. Extra characters.";
    const char *long_str6 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    assert(fast_strcmp(long_str5, long_str6) == (' ' - '\0'));

    const char *long_str7 = "This is a very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    const char *long_str8 = "Another very long string used for testing purposes. "
                            "It contains multiple sentences and is designed to test "
                            "the performance and correctness of the fast_strcmp function.";
    assert(fast_strcmp(long_str7, long_str8) == ('T' - 'A'));
}

int main(void)
{
    test_fast_strcmp();
	printf("All tests passed for fast_strcmp\n");
    return 0;
}