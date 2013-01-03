#include "config.h"

#undef config_read
int config_read(parser_t *parser, char **tokens, unsigned flags, const char *delims) {
    return 0;
}

void config_close(parser_t *parser) {
   	if (parser) {
		free(parser->line);
		free(parser->nline);
		free(parser);
	}
}

parser_t* config_open(const char *filename) {
	parser_t *parser;
	parser = (parser_t *) malloc(sizeof(*parser));
	parser->fp = NULL;
	return parser;
}
