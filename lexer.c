
/*	---------------------------------------------
 * 	| ls -l | wc -l 							|
 *	---------------------------------------------
 *
 *	------------------------------------------------------------
 *	| ls | \0 | -l | \0 | | | \0 | wc | \0 | -l | \0 | \n | \0 |
 *	------------------------------------------------------------
 *	^			^----    ^
 *	|-----------	 |   |
 *				|	 |   |
 *	+--------+	|	 |   |
 *	|		-|--	 |   |
 *	+--------+		 |   |
 *	|		-|--------	 |
 *	+--------+			 |
 *	|		-|------------
 *	+--------+
 *
 *
 *
 *
 *
 */

#include "lib.h"

//return the end of words
static char *pdst;
static char *porig;

char* next_token()
{
	char *pw;
	char c, cc, peek;

	pw = pdst;

	while((*porig == ' ') || (*porig == '\t')) {
		porig++;
	}

	while (1) {
		c = *porig;
		
		if (in_special(c, " \t;&<>()|\n")) {
			if (c == ' ' || c == '\t') {
				porig++;
				continue;
			}
			porig++;
			*pdst++ = c;
			*pdst++ = '\0';
			return pw;
		}
		if ((c == '\'') || (c == '"')) {
			cc = *++porig;
			while (cc != c) {
				if (cc == '\n') {
					goto error;
				}
				*pdst++ = cc;
				cc = *++porig;
			}
			c = *++porig;
			if (in_special(c, " \t;&<>()|\n")) {
				*pdst++ = '\0';
				return pw;
			}
		}
		*pdst++ = c;
		//peek = *(porig + 1);
		peek = *++porig;
		if (in_special(peek, " \t;&<>()|\n")) {
			*pdst++ = '\0';
			return pw;
		}

	}
	
error:
	shell_error = 1; 	//lex error
	return NULL;
}

char** lexer(char *orig, char *dst, char **words)
{
	int idx;

	idx = 0;
	porig = orig;
	pdst = dst;

	while (1) {
		words[idx] = next_token();
		if (words[idx] == NULL) {
			return NULL;
		}
		if (strcmp(words[idx], "\n") == 0) {
			return &words[idx + 1];
		}
		idx++;
	}
}
