#include <stdlib.h>
#include <string.h>

/*
	* Convert a string to lower case (copy, so remember to free)
	*/
char *str_tolower(const char *str)
{
				char *lower_str;
				int len;
				int i;

				len = strlen(str) + 1;
				lower_str = malloc(len);
				memset(lower_str, 0, len);
				if (lower_str == NULL) {
								return NULL;
				}

				for (i = 0; str[i]; i++) {
								lower_str[i] = tolower(str[i]);
				}

				return lower_str;
}

