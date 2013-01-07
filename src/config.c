#include "config.h"
#include <string.h>
#include "baby-cron.h"

parser_t* FAST_FUNC config_open2(const char *filename, FILE* FAST_FUNC (*fopen_func)(const char *path))
{
	FILE* fp;
	parser_t *parser;

	fp = fopen_func(filename);
	if (!fp)
		return NULL;
    parser = (parser_t* )xzalloc(sizeof(*parser));
	parser->fp = fp;
	return parser;
}

parser_t* FAST_FUNC config_open(const char *filename)
{
	return config_open2(filename, fopen_or_warn_stdin);
}

void FAST_FUNC config_close(parser_t *parser)
{
	if (parser) {
		if (PARSE_KEEP_COPY) /* compile-time constant */
			free(parser->data);
		fclose(parser->fp);
		free(parser->line);
		free(parser->nline);
		free(parser);
	}
}

/* This function reads an entire line from a text file,
 * up to a newline, exclusive.
 * Trailing '\' is recognized as line continuation.
 * Returns -1 if EOF/error.
 */
static int get_line_with_continuation(parser_t *parser)
{
	ssize_t len, nlen;
	char *line;

	len = getline(&parser->line, &parser->line_alloc, parser->fp);
	if (len <= 0)
		return len;

	line = parser->line;
	for (;;) {
		parser->lineno++;
		if (line[len - 1] == '\n')
			len--;
		if (len == 0 || line[len - 1] != '\\')
			break;
		len--;

		nlen = getline(&parser->nline, &parser->nline_alloc, parser->fp);
		if (nlen <= 0)
			break;

		if (parser->line_alloc < (unsigned)(len + nlen + 1)) {
			parser->line_alloc = len + nlen + 1;
			line = parser->line = (char *)xrealloc(line, parser->line_alloc);
		}
		memcpy(&line[len], parser->nline, nlen);
		len += nlen;
	}

	line[len] = '\0';
	return len;
}


/*
0. If parser is NULL return 0.
1. Read a line from config file. If nothing to read then return 0.
   Handle continuation character. Advance lineno for each physical line.
   Discard everything past comment character.
2. if PARSE_TRIM is set (default), remove leading and trailing delimiters.
3. If resulting line is empty goto 1.
4. Look for first delimiter. If !PARSE_COLLAPSE or !PARSE_TRIM is set then
   remember the token as empty.
5. Else (default) if number of seen tokens is equal to max number of tokens
   (token is the last one) and PARSE_GREEDY is set then the remainder
   of the line is the last token.
   Else (token is not last or PARSE_GREEDY is not set) just replace
   first delimiter with '\0' thus delimiting the token.
6. Advance line pointer past the end of token. If number of seen tokens
   is less than required number of tokens then goto 4.
7. Check the number of seen tokens is not less the min number of tokens.
   Complain or die otherwise depending on PARSE_MIN_DIE.
8. Return the number of seen tokens.

mintokens > 0 make config_read() print error message if less than mintokens
(but more than 0) are found. Empty lines are always skipped (not warned about).
*/
#undef config_read
int config_read(parser_t *parser, char **tokens, unsigned flags, const char *delims)
{
	char *line;
	int ntokens, mintokens;
	int t;

	if (!parser)
		return 0;

	ntokens = (uint8_t)flags;
	mintokens = (uint8_t)(flags >> 8);
    ntokens = 6;
    mintokens = 1;
    
 again:
	memset(tokens, 0, sizeof(tokens[0]) * ntokens);

	/* Read one line (handling continuations with backslash) */
	if (get_line_with_continuation(parser) < 0)
		return 0;

	line = parser->line;

	/* Skip token in the start of line? */
	if (flags & PARSE_TRIM)
		line += strspn(line, delims + 1);

	if (line[0] == '\0' || line[0] == delims[0])
		goto again;

	if (flags & PARSE_KEEP_COPY) {
		free(parser->data);
		parser->data = xstrdup(line);
	}

	/* Tokenize the line */
	t = 0;
	do {
		/* Pin token */
		tokens[t] = line;

		/* Combine remaining arguments? */
		if ((t != (ntokens-1)) || !(flags & PARSE_GREEDY)) {
			/* Vanilla token, find next delimiter */
			line += strcspn(line, delims[0] ? delims : delims + 1);
		} else {
			/* Combining, find comment char if any */
			line = strchrnul(line, PARSE_EOL_COMMENTS ? delims[0] : '\0');

			/* Trim any extra delimiters from the end */
			if (flags & PARSE_TRIM) {
				while (strchr(delims + 1, line[-1]) != NULL)
					line--;
			}
		}

		/* Token not terminated? */
		if (*line == delims[0])
			*line = '\0';
		else if (*line != '\0')
			*line++ = '\0';

#if 0 /* unused so far */
		if (flags & PARSE_ESCAPE) {
			strcpy_and_process_escape_sequences(tokens[t], tokens[t]);
		}
#endif
		/* Skip possible delimiters */
		if (flags & PARSE_COLLAPSE)
			line += strspn(line, delims + 1);

		t++;
	} while (*line != delims[0] && t < ntokens); //TODO: conform *line is not needed

	if (t < mintokens) {
	//	bb_error_msg("bad line %u: %d tokens found, %d needed",
    //				parser->lineno, t, mintokens);
    //				TODO: replace with shakespeare
		//if (flags & PARSE_MIN_DIE)
		//	xfunc_die();
		//	TODO: do we need to hanfle mintokens??
		goto again;
	}

	return t;
}
