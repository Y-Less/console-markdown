extern "C"
{
	#include "libsoldout/markdown.h"
}

#include <stack>
#include <iostream>

static int RenderMarkdown(void * opaque, char const * style, struct buf const * text)
{
	if (text && text->size)
	{
		::std::ostream &
			out = *reinterpret_cast<::std::ostream *>(opaque);
		out << style;
		out.write(text->data, text->size);
		out << "\x1B[0m";
	}
	return 0;
}

static int RenderMarkdown(void * opaque, struct buf const * text)
{
	if (text && text->size)
	{
		::std::ostream &
			out = *reinterpret_cast<::std::ostream *>(opaque);
		out.write(text->data, text->size);
	}
	return 0;
}

/*static void
	rndr_fencedcode(struct buf *ob, struct buf *text, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	BUFPUTSL(ob, "<pre><code>");
	if (text)
	{
		lus_body_escape(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</code></pre>\n");
}

static void
	rndr_blockquote(struct buf *ob, struct buf *text, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	BUFPUTSL(ob, "<blockquote>\n");
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</blockquote>\n");
}

static int
	rndr_codespan(struct buf *ob, struct buf *text, void *opaque)
{
	BUFPUTSL(ob, "<code>");
	if (text)
	{
		lus_body_escape(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</code>");
	return 1;
}*/

static int
	rndr_double_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Double `*` or `_` is `__bold__`.
	return RenderMarkdown(opaque, "\x1B[5;47;30m", text);
}

static int
	rndr_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Single `*` or `_` is `*italic*`.
	return RenderMarkdown(opaque, "\x1B[37;1m", text);
}

/*static void
	rndr_header(struct buf *ob, struct buf *text, int level, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	bufprintf(ob, "<h%d>", level);
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	bufprintf(ob, "</h%d>\n", level);
}

static int
	rndr_link(struct buf *ob, struct buf *link, struct buf *title, struct buf *content, void *opaque)
{
	BUFPUTSL(ob, "<a href=\"");
	if (link && link->size)
	{
		lus_attr_escape(ob, link->data, link->size);
	}
	if (title && title->size)
	{
		BUFPUTSL(ob, "\" title=\"");
		lus_attr_escape(ob, title->data, title->size);
	}
	BUFPUTSL(ob, "\">");
	if (content && content->size)
	{
		bufput(ob, content->data, content->size);
	}
	BUFPUTSL(ob, "</a>");
	return 1;
}

static void
	rndr_list(struct buf *ob, struct buf *text, int flags, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	bufput(ob, (flags & MKD_LIST_ORDERED) ? "<ol>\n" : "<ul>\n", 5);
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	bufput(ob, (flags & MKD_LIST_ORDERED) ? "</ol>\n" : "</ul>\n", 6);
}

static void
	rndr_listitem(struct buf *ob, struct buf *text, int flags, void *opaque)
{
	BUFPUTSL(ob, "<li>");
	if (text)
	{
		while (text->size && text->data[text->size - 1] == '\n')
		{
			text->size -= 1;
		}
		bufput(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</li>\n");
}*/

static void
	rndr_normal_text(struct buf *ob, struct buf *text, void *opaque)
{
	RenderMarkdown(opaque, text);
}

/*static void
	rndr_paragraph(struct buf *ob, struct buf *text, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	BUFPUTSL(ob, "<p>");
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</p>\n");
}

static void
	rndr_raw_block(struct buf *ob, struct buf *text, void *opaque)
{
	size_t org, sz;
	if (!text)
	{
		return;
	}
	sz = text->size;
	while (sz > 0 && text->data[sz - 1] == '\n')
	{
		sz -= 1;
	}
	org = 0;
	while (org < sz && text->data[org] == '\n')
	{
		org += 1;
	}
	if (org >= sz)
	{
		return;
	}
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	bufput(ob, text->data + org, sz - org);
	bufputc(ob, '\n');
}

static int
	rndr_raw_inline(struct buf *ob, struct buf *text, void *opaque)
{
	bufput(ob, text->data, text->size);
	return 1;
}*/

/* exported renderer structures */
void WriteMD(char const * str)
{
	static struct mkd_renderer
		consoleRenderer =
		{
			NULL,
			NULL,

			NULL,
			NULL, //rndr_fencedcode,
			NULL, //discount_blockquote,
			NULL, //rndr_raw_block,
			NULL, //rndr_header,
			NULL, //html_hrule,
			NULL, //rndr_list,
			NULL, //rndr_listitem,
			NULL, //rndr_paragraph,
			NULL, //discount_table,
			NULL, //discount_table_cell,
			NULL, //discount_table_row,

			NULL,
			NULL, //rndr_codespan,
			rndr_double_emphasis,
			rndr_emphasis,
			NULL, //html_discount_image,
			NULL, //html_linebreak,
			NULL, //discount_link,
			NULL, //rndr_raw_inline,
			NULL, //rndr_triple_emphasis,

			NULL,
			rndr_normal_text,

			64,
			"*_",
			NULL
		};
	::std::ostream *
		out = &::std::cout;
	buf
		* ob = bufnew(64),
		ib = {
			const_cast<char *>(str),
			strlen(str),
			0,
			0,
			0xFFFF
		};
	//markdown(ob, ib, &to_man);

	/* writing the result to stdout */
	//ret = fwrite(ob->data, 1, ob->size, stdout);
	//if (ret < ob->size)
	//	fprintf(stderr, "Warning: only %zu output byte written, "
	//		"out of %zu\n",
	//		ret,
	//		ob->size);
	consoleRenderer.opaque = reinterpret_cast<void *>(out);
	markdown(ob, &ib, &consoleRenderer);

	/* cleanup */
	//bufrelease(ib);
	bufrelease(ob);
}

