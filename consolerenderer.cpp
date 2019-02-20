extern "C"
{
	#include "libsoldout/markdown.h"
}

#include <stack>
#include <iostream>
#include <string>

#include "parsers/cpp.hpp"
#include "parsers/pawn.hpp"

static int RenderMarkdown(struct buf * ob, char const * style, struct buf const * text)
{
	if (text && text->size)
	{
		//::std::ostream &
		//	out = *reinterpret_cast<::std::ostream *>(opaque);
		//out << style;
		size_t
			len = strlen(style);
		bufput(ob, style, len);
		bufput(ob, text->data, text->size);
		BUFPUTSL(ob, "\x1B[0m");
		// `4` for `strlen("\x1B[0m")`;
		//return len + text->size + 4;
		return 1;
	}
	return 0;
}

static int RenderText(struct buf * ob, struct buf const * text)
{
	if (text && text->size)
	{
		bufput(ob, text->data, text->size);
		//return text->size;
		return 1;
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
	return RenderMarkdown(ob, "\x1B[5;47;30m", text);
}

static int
	rndr_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Single `*` or `_` is `*italic*`.
	return RenderMarkdown(ob, "\x1B[37;1m", text);
}

void rndr_blockcode(struct buf *ob, struct buf *text, void *opaque)
{
	// Just indent it all 4 spaces.
	BUFPUTSL(ob, "\n");
}

void rndr_fencedcode(struct buf *ob, struct buf *text, char *name, size_t namelen, void *opaque)
{
	if (!text || !text->size)
	{
		return;
	}
	if (namelen)
	{
		if (!strncmp(name, "cpp", 3))
		{
			::std::string
				out = Syntax::CPP(::std::string(text->data, text->size));
			BUFPUTSL(ob, "\n");
			bufput(ob, out.c_str(), out.length());
		}
		else if (!strncmp(name, "pawn", 3))
		{
			::std::string
				out = Syntax::Pawn(::std::string(text->data, text->size));
			BUFPUTSL(ob, "\n");
			bufput(ob, out.c_str(), out.length());
		}
		else
		{
			rndr_blockcode(ob, text, opaque);
		}
	}
}

static void
	rndr_header(struct buf *ob, struct buf *text, int level, void *opaque)
{
	static char const
		firstLevel[] = "================================================================================",
		secondLevel[] = "------------------------------------------------------------------------------";
	if (text && text->size)
	{
		if (ob->size)
		{
			bufputc(ob, '\n');
		}
		size_t
			len = text->size;
		if (len > 78)
		{
			len = 78;
		}
		switch (level)
		{
		case 1:
			BUFPUTSL(ob, "\x1B[42;5;30m ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, " \n");
			bufput(ob, firstLevel, len + 2);
			BUFPUTSL(ob, "\x1B[0m\n");
			break;
		case 2:
			BUFPUTSL(ob, "\x1B[32;1m ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, "\n ");
			bufput(ob, secondLevel, len);
			BUFPUTSL(ob, "\x1B[0m\n");
			break;
		case 3:
			BUFPUTSL(ob, "\x1B[33;1m ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, "\x1B[0m\n");
			break;
		case 4:
			BUFPUTSL(ob, "\x1B[31;1m   ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, "\x1B[0m\n");
			break;
		}
	}
}

/*static int
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
}*/

static void
	rndr_list(struct buf *ob, struct buf *text, int *flags, void *opaque)
{
	//bufput(ob, (flags & MKD_LIST_ORDERED) ? "<ol>\n" : "<ul>\n", 5);
	if (text)
	{
		if (ob->size)
		{
			bufputc(ob, '\n');
		}
		bufput(ob, text->data, text->size);
	}
	printf("a flags %d", *flags);
	//bufput(ob, (flags & MKD_LIST_ORDERED) ? "</ol>\n" : "</ul>\n", 6);
}

static void
	rndr_listitem(struct buf *ob, struct buf *text, int *flags, void *opaque)
{
	int pos = 0;
	char num[4];
	if (text)
	{
		while (text->size && text->data[text->size - 1] == '\n')
		{
			text->size -= 1;
		}
		if (text->size)
		{
			if (*flags & MKD_LIST_ORDERED)
			{
				pos = *flags >> 8;
				pos += 1;
				*flags = (*flags & 0xFF) | (pos << 8);
				BUFPUTSL(ob, " \x1B[35m");
				sprintf_s(num, "%d", pos);
				bufput(ob, num, strlen(num));
				BUFPUTSL(ob, ")\x1B[0m ");
				bufput(ob, text->data, text->size);
				BUFPUTSL(ob, "\n");
			}
			else
			{
				BUFPUTSL(ob, " \x1B[35m*\x1B[0m ");
				bufput(ob, text->data, text->size);
				BUFPUTSL(ob, "\n");
			}
		}
	//if (text)
	//{
	//	while (text->size && text->data[text->size - 1] == '\n')
	//	{
	//		text->size -= 1;
	//	bufput(ob, text->data, text->size);
	}
	//BUFPUTSL(ob, "</li>\n");
}

static void
	rndr_normal_text(struct buf *ob, struct buf *text, void *opaque)
{
	RenderText(ob, text);
}

static void
	null_block(struct buf *ob, struct buf *text, void *opaque)
{
	RenderText(ob, text);
}

static int
	null_span(struct buf *ob, struct buf *text, void *opaque)
{
	RenderText(ob, text);
	return 1;
}

static void
	rndr_paragraph(struct buf *ob, struct buf *text, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	if (text)
	{
		bufput(ob, text->data, text->size);
	}
	BUFPUTSL(ob, "\n");
}

/*static void
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

static int
	rndr_linebreak(struct buf *ob, void *opaque)
{
	BUFPUTSL(ob, "\n");
	return 1;
}

void rndr_hrule(struct buf *ob, void *opaque)
{
	BUFPUTSL(ob, "--------------------------------------------------------------------------------");
}

/* exported renderer structures */
void WriteMD(char const * str)
{
	static const struct mkd_renderer
		consoleRenderer =
		{
			NULL,
			NULL,

			rndr_blockcode,//NULL,
			rndr_fencedcode, //rndr_fencedcode,
			null_block,//NULL, //discount_blockquote,
			null_block,//NULL, //rndr_raw_block,
			rndr_header, //rndr_header,
			rndr_hrule, //html_hrule,
			rndr_list,
			rndr_listitem,
			rndr_paragraph,//NULL, //rndr_paragraph,
			NULL, //discount_table,
			NULL, //discount_table_cell,
			NULL, //discount_table_row,

			NULL,
			NULL, //rndr_codespan,
			rndr_double_emphasis,
			rndr_emphasis,
			NULL, //html_discount_image,
			rndr_linebreak,
			NULL, //discount_link,
			null_span,//NULL, //rndr_raw_inline,
			NULL, //rndr_triple_emphasis,

			NULL,
			NULL,

			64,
			"*_",
			NULL
		};
	//::std::ostream *
	//	out = &::std::cout;
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
	//consoleRenderer.opaque = reinterpret_cast<void *>(out);
	markdown(ob, &ib, &consoleRenderer);

	::std::cout.write(ob->data, ob->size);

	/* cleanup */
	//bufrelease(ib);
	bufrelease(ob);
}

