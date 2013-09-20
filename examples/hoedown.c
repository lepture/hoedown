#include "markdown.h"
#include "html.h"
#include "buffer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

/* main • main function, interfacing STDIO with the parser */
int
main(int argc, char **argv)
{
	struct hoedown_buffer *ib, *ob;
	int ret;
	FILE *in = stdin;

	struct hoedown_callbacks callbacks;
	struct hoedown_html_renderopt options;
	struct hoedown_markdown *markdown;

	/* opening the file if given from the command line */
	if (argc > 1) {
		in = fopen(argv[1], "r");
		if (!in) {
			fprintf(stderr,"Unable to open input file \"%s\": %s\n", argv[1], strerror(errno));
			return 1;
		}
	}

	/* reading everything */
	ib = hoedown_buffer_new(READ_UNIT);
	hoedown_buffer_grow(ib, READ_UNIT);
	while ((ret = fread(ib->data + ib->size, 1, ib->asize - ib->size, in)) > 0) {
		ib->size += ret;
		hoedown_buffer_grow(ib, ib->size + READ_UNIT);
	}

	if (in != stdin)
		fclose(in);

	/* performing markdown parsing */
	ob = hoedown_buffer_new(OUTPUT_UNIT);

	hoedown_html_renderer(&callbacks, &options, 0);
	markdown = hoedown_markdown_new(0, 16, &callbacks, &options);

	hoedown_markdown_render(ob, ib->data, ib->size, markdown);
	hoedown_markdown_free(markdown);

	/* writing the result to stdout */
	ret = fwrite(ob->data, 1, ob->size, stdout);

	/* cleanup */
	hoedown_buffer_release(ib);
	hoedown_buffer_release(ob);

	return (ret < 0) ? -1 : 0;
}

/* vim: set filetype=c: */
