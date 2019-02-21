extern "C"
{
	#include "libsoldout/markdown.h"
}

#include <stack>
#include <iostream>
#include <string>

#include "ColouredBuffer.hpp"
#include "include/CMDMD/CMDMD.hpp"

#include "StringUtils.hpp"
#include "parsers/cpp.hpp"
#include "parsers/pawn.hpp"

static int RenderMarkdown(struct buf * ob, char const * style, struct buf const * text)
{
	if (text && text->size)
	{
		size_t
			len = strlen(style);
		bufput(ob, style, len);
		bufput(ob, text->data, text->size);
		BUFPUTSL(ob, "\x1B[0m");
		return 1;
	}
	return 0;
}

struct console_data_s
{
	size_t Width;
	int BulletPoint;
};

/*static void
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
}*/

static int
	rndr_codespan(struct buf *ob, struct buf *text, void *opaque)
{
	if (text)
	{
		BUFPUTSL(ob, "\x1B[47;30m");
		bufput(ob, text->data, text->size);
		BUFPUTSL(ob, "\x1B[0m");
	}
	return 1;
}

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
	if (text && text->size)
	{
		::std::string
			code(text->data, text->size);

		std::string::size_type
			lastPos = 0,
			findPos;

		size_t
			width = reinterpret_cast<console_data_s *>(opaque)->Width;
		if (ob->size)
		{
			BUFPUTSL(ob, "\n");
		}
		while (std::string::npos != (findPos = code.find("\n", lastPos)))
		{
			BUFPUTSL(ob, "\x1B[0m    \x1B[47;30m");
			bufput(ob, code.c_str() + lastPos, findPos - lastPos);
			if (findPos - lastPos < width - 4)
			{
				bufputcn(ob, ' ', width - 4 - findPos + lastPos);
			}
			else
			{
				BUFPUTSL(ob, "\n");
			}
			lastPos = findPos + 1;
		}
		bufput(ob, code.c_str() + lastPos, code.length() - lastPos);
		BUFPUTSL(ob, "\x1B[0m\n");
	}
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
			return;
		}
		else if (!strncmp(name, "pawn", 3))
		{
			::std::string
				out = Syntax::Pawn(::std::string(text->data, text->size));
			BUFPUTSL(ob, "\n");
			bufput(ob, out.c_str(), out.length());
			return;
		}
	}
	rndr_blockcode(ob, text, opaque);
}

static void
	rndr_header(struct buf *ob, struct buf *text, int level, void *opaque)
{
	if (text && text->size)
	{
		if (ob->size)
		{
			BUFPUTSL(ob, "\n");
		}
		size_t
			width = reinterpret_cast<console_data_s *>(opaque)->Width,
			len = text->size;
		if (len > width - 2)
		{
			len = width - 2;
		}
		switch (level)
		{
		case 1:
			BUFPUTSL(ob, "\x1B[42;5;30m ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, " \n");
			bufputcn(ob, '=', len + 2);
			BUFPUTSL(ob, "\x1B[0m\n");
			break;
		case 2:
			BUFPUTSL(ob, "\x1B[32;1m ");
			bufput(ob, text->data, text->size);
			BUFPUTSL(ob, "\n ");
			bufputcn(ob, '-', len);
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
	rndr_list(struct buf *ob, struct buf *text, int flags, void *opaque)
{
	if (text)
	{
		if (ob->size)
		{
			BUFPUTSL(ob, "\n");
		}
		bufput(ob, text->data, text->size);
		reinterpret_cast<console_data_s *>(opaque)->BulletPoint = 0;
	}
}

static void
	rndr_listitem(struct buf *ob, struct buf *text, int flags, void *opaque)
{
	char num[4];
	if (text)
	{
		while (text->size && text->data[text->size - 1] == '\n')
		{
			text->size -= 1;
		}
		if (text->size)
		{
			if (flags & MKD_LIST_ORDERED)
			{
				BUFPUTSL(ob, " \x1B[35m");
				sprintf_s(num, "%d", ++reinterpret_cast<console_data_s *>(opaque)->BulletPoint);
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
	}
}

static void
	rndr_normal_text(struct buf *ob, struct buf *text, void *opaque)
{
	bufput(ob, text->data, text->size);
}

static void
	null_block(struct buf *ob, struct buf *text, void *opaque)
{
	bufput(ob, text->data, text->size);
}

static int
	null_span(struct buf *ob, struct buf *text, void *opaque)
{
	bufput(ob, text->data, text->size);
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
	if (ob->size)
	{
		BUFPUTSL(ob, "\n");
	}
	size_t
		len = reinterpret_cast<console_data_s *>(opaque)->Width;
	BUFPUTSL(ob, "\x1B[47;37m");
	bufputcn(ob, ' ', len);
	BUFPUTSL(ob, "\x1B[0m\n");
}

/* exported renderer structures */
::std::string cmdmd::Render(std::string const & input)
{
	return cmdmd::Render(input.c_str(), input.length());
}

::std::string cmdmd::Render(char const * input)
{
	return cmdmd::Render(input, strlen(input));
}

::std::string cmdmd::Render(char const * input, size_t len)
{
	static struct mkd_renderer
		consoleRenderer =
		{
			NULL,
			NULL,

			rndr_blockcode,//NULL,
			rndr_fencedcode,
			null_block,//NULL, //discount_blockquote,
			null_block,//NULL, //rndr_raw_block,
			rndr_header,
			rndr_hrule,
			rndr_list,
			rndr_listitem,
			rndr_paragraph,
			NULL, //discount_table,
			NULL, //discount_table_cell,
			NULL, //discount_table_row,

			NULL,
			rndr_codespan,
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
	buf
		* ob = bufnew(64),
		ib = {
			const_cast<char *>(input),
			len,
			0,
			0,
			0xFFFF
		};
#ifdef CONMD_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO
		console;
	size_t
		width = 80;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console))
	{
		width = console.dwSize.X;
	}
	console_data_s
		data = { width, 0 };
#else
	console_data_s
		data = { 80, 0 };
#endif
	consoleRenderer.opaque = reinterpret_cast<void *>(&data);
	markdown(ob, &ib, &consoleRenderer);

	::std::string
		ret(ob->data, ob->size);

	bufrelease(ob);

	return ret;
}

void cmdmd::Init()
{
	ColouredBuffer<char>::StandardInstall();
	ColouredBuffer<wchar_t>::StandardInstall();
}

::std::string
	cmdmd::
	Literals::
	operator "" _cmdmd(char const * s, size_t len)
{
	return ::cmdmd::Render(s, len);
}

