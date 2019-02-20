extern "C"
{
	#include "libsoldout/markdown.h"
}

#include <stack>
#include <iostream>

/********************
* GENERIC RENDERER *
********************/

class CMDMDStack
{
private:
	::std::ostream & out_;

	::std::stack<char const *> styles_;

public:
	CMDMDStack(::std::ostream & out)
	:
		out_(out),
		styles_()
	{
		// Default colours.
		Push("\x1B[0m");
	}

	~CMDMDStack()
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

	inline int operator()(struct buf *text)
	{
		if (text && text->size)
		{
			out_.write(text->data, text->size);
		}
		return 0;
	}
};

class CMDMD
{
private:
	CMDMDStack &
		stack_;

public:
	CMDMD(char const * style, void * stack)
	:
		stack_(*reinterpret_cast<CMDMDStack *>(stack))
	{
		stack_.Push(style);
	}
	~CMDMD()
	{
		stack_.Pop();
	}

	inline int operator()(struct buf *text)
	{
		return stack_(text);
	}
};

class CMDTX
{
private:
	CMDMDStack &
		stack_;

public:
	CMDTX(void * stack)
	:
		stack_(*reinterpret_cast<CMDMDStack *>(stack))
	{
	}

	inline int operator()(struct buf *text)
	{
		return stack_(text);
	}
};

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
	if (!text || !text->size)
	{
		return 0;
	}
	return CMDMD("\x1B[5;47;30m", opaque)(text);
}

static int
	rndr_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Single `*` or `_` is `*italic*`.
	if (!text || !text->size)
	{
		return 0;
	}
	return CMDMD("\x1B[37;1m", opaque)(text);
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
	CMDTX(opaque)(text);
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
	CMDMDStack
		stack(::std::cout);
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
	consoleRenderer.opaque = reinterpret_cast<void *>(&stack);
	markdown(ob, &ib, &consoleRenderer);

	/* cleanup */
	//bufrelease(ib);
	bufrelease(ob);
}

