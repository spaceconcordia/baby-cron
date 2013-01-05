#include "config.h"
#include "config-stub.h"
#include <cstring>

static char **configTokens = NULL;
static char *configLine;
void set_config(char **tokens, char *line) {
    configTokens = tokens;
    configLine   = line;
}

#undef config_read
/*
 * The config_read reads the lines from the crontab file. This stub assign on line
 * if config_read is called the first time. Line is set to NULL and 0 is return to
 * mean the end of file has been reached.
 * */
int config_read(parser_t *parser, char **tokens, unsigned flags, const char *delims) {    	
	int t = 6;

	if (!parser) { return 0; }

    if (parser->line != NULL) { 
        free(parser->line);
        parser->line = NULL;
        return 0; 
    }
   
    for (int i = 0; i != t; i++) {
        tokens[i] = configTokens[i];
    }

    parser->line = configLine;
	return t;
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
