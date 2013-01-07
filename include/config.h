#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"

#define ENABLE_FEATURE_CROND_D 0

enum {
	PARSE_COLLAPSE  = 0x00010000, // treat consecutive delimiters as one
	PARSE_TRIM      = 0x00020000, // trim leading and trailing delimiters
// TODO: COLLAPSE and TRIM seem to always go in pair
	PARSE_GREEDY    = 0x00040000, // last token takes entire remainder of the line
	PARSE_MIN_DIE   = 0x00100000, // die if < min tokens found
	// keep a copy of current line
	PARSE_KEEP_COPY = 0x00200000 * ENABLE_FEATURE_CROND_D,
	PARSE_EOL_COMMENTS = 0x00400000, // comments are recognized even if they aren't the first char
	// NORMAL is:
	// * remove leading and trailing delimiters and collapse
	//   multiple delimiters into one
	// * warn and continue if less than mintokens delimiters found
	// * grab everything into last token
	// * comments are recognized even if they aren't the first char
	PARSE_NORMAL    = PARSE_COLLAPSE | PARSE_TRIM | PARSE_GREEDY | PARSE_EOL_COMMENTS
};

struct parser_t {
    FILE *fp;
    char *data;
    char *line, *nline;
    size_t line_alloc, nline_alloc;
    int lineno;
};
 
int config_read(parser_t *parser, char **tokens, unsigned flags, const char *delims);
#define config_read(parser, tokens, max, min, str, flags) \
	config_read(parser, tokens, ((flags) | (((min) & 0xFF) << 8) | ((max) & 0xFF)), str)

void config_close(parser_t *parser) FAST_FUNC;
parser_t* config_open(const char *filename) FAST_FUNC;

#endif
