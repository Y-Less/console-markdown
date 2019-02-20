#include "libsoldout/markdown.h"

#include <stack>
#include <ostream>

/********************
* GENERIC RENDERER *
********************/

class CMDMD
{
private:
	::std::ostream & out_;

	::std::stack<char const *> styles_;

public:
	CMDMD(::std::ostream & out)
	:
		out_(out),
		styles_()
	{
		// Default colours.
		Push("\x1B[0m");
	}

	~CMDMD()
	{
		// Reset.
		out_ << "\x1B[0m";
	}

	void Push(char const * style)
	{
		out_ << style;
		styles_.push(style);
	}

	void Pop()
	{
		styles_.pop();
		out_ << styles_.top();
	}
};

static int
	rndr_autolink(struct buf *ob, struct buf *link, enum mkd_autolink type, void *opaque)
{
	if (!link || !link->size)
	{
		return 0;
	}
	BUFPUTSL(ob, "<a href=\"");
	if (type == MKDA_IMPLICIT_EMAIL)
	{
		BUFPUTSL(ob, "mailto:");
	}
	lus_attr_escape(ob, link->data, link->size);
	BUFPUTSL(ob, "\">");
	if (type == MKDA_EXPLICIT_EMAIL && link->size > 7)
	{
		lus_body_escape(ob, link->data + 7, link->size - 7);
	}
	else
	{
		lus_body_escape(ob, link->data, link->size);
	}
	BUFPUTSL(ob, "</a>");
	return 1;
}

static void
	rndr_blockcode(struct buf *ob, struct buf *text, void *opaque)
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
}

static int
	rndr_double_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	CMDMD *
		styles = (cmdmd_s *)opaque;
	// Double `*` or `_` is `__bold__`.
	if (!text || !text->size)
	{
		return 0;
	}
	if (c != '*')
	{
		return 0;
	}
	BUFPUTSL(ob, "<strong>");
	bufput(ob, text->data, text->size);
	BUFPUTSL(ob, "</strong>");
	return 1;
}

static int
	rndr_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Single `*` or `_` is `*italic*`.
	if (!text || !text->size)
	{
		return 0;
	}
	BUFPUTSL(ob, "<em>");
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "</em>");
	return 1;
}

static void
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
}

static void
	rndr_normal_text(struct buf *ob, struct buf *text, void *opaque)
{
	if (text)
	{
		lus_body_escape(ob, text->data, text->size);
	}
}

static void
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
}

/*static int
	rndr_triple_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	if (!text || !text->size)
	{
		return 0;
	}
	BUFPUTSL(ob, "<strong><em>");
	bufput(ob, text->data, text->size);
	BUFPUTSL(ob, "</em></strong>");
	return 1;
}*/

/* exported renderer structures */
const struct mkd_renderer gConsoleRenderer =
{
	NULL,
	NULL,

	rndr_blockcode,
	NULL,
	discount_blockquote,
	rndr_raw_block,
	rndr_header,
	html_hrule,
	rndr_list,
	rndr_listitem,
	rndr_paragraph,
	discount_table,
	discount_table_cell,
	discount_table_row,

	rndr_autolink,
	rndr_codespan,
	rndr_double_emphasis,
	rndr_emphasis,
	html_discount_image,
	html_linebreak,
	discount_link,
	rndr_raw_inline,
	NULL, //rndr_triple_emphasis,

	NULL,
	rndr_normal_text,

	64,
	"*_",
	NULL
};

