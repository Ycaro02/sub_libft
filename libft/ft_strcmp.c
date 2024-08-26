int	ftlib_strcmp(char *s1, char *s2)
{
	int	i;
	int	j;

	j = 0;
	i = 0;
	while (s1[i] == s2[j] && (s1[i] != '\0' && s2[j] != '\0'))
	{
		i++;
		j++;
	}
	if (s2[j] == '\n')
		j++;
	return (s1[i] - s2[j]);
}

#include <stdint.h>

int fast_strcmp(const char *s1, const char *s2)
{
	const uint64_t *p1, *p2;
	
    /* Align to 8-byte boundary */
    while (((uintptr_t)s1 & 7) && *s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }

	/* Compare 8 bytes at a time */
	p1 = (const uint64_t *)s1;
	p2 = (const uint64_t *)s2;
    while (*p1 == *p2)
    {
        if (((*p1 - 0x0101010101010101ULL) & ~*p1 & 0x8080808080808080ULL) != 0)
            break;
        p1++;
        p2++;
    }

	/* Compare the remaining bytes */
    s1 = (const char *)p1;
    s2 = (const char *)p2;
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    return (*(const unsigned char *)s1 - *(const unsigned char *)s2);
}