extern "C"
{
	#include "libsoldout/markdown.h"
}

#include <stack>
#include <iostream>
#include <string>

#include "ColouredBuffer.hpp"
#include "include/console-markdown/CMDMD.hpp"

#include "include/console-markdown/CPP.hpp"
#include "include/console-markdown/Pawn.hpp"

#ifndef CONMD_WINDOWS
	#include <sys/ioctl.h>
	#include <unistd.h>
	#define sprintf_s sprintf
#endif

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
	size_t WindowWidth;
	size_t BufferWidth;
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
#ifndef CONMD_WINDOWS
	return RenderMarkdown(ob, "\x1B[5;47;30m", text);
#else
	return RenderMarkdown(ob, "\x1B[1m", text);
#endif
}

static int
	rndr_emphasis(struct buf *ob, struct buf *text, char c, void *opaque)
{
	// Single `*` or `_` is `*italic*`.
#ifdef CONMD_WINDOWS
	return RenderMarkdown(ob, "\x1B[37;1m", text);
#else
	return RenderMarkdown(ob, "\x1B[4m", text);
#endif
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
			windowWidth = reinterpret_cast<console_data_s *>(opaque)->WindowWidth;
		bool
			nl = windowWidth < reinterpret_cast<console_data_s *>(opaque)->BufferWidth;
		if (ob->size)
		{
			bufputc(ob, '\n');
		}
		while (std::string::npos != (findPos = code.find("\n", lastPos)))
		{
			BUFPUTSL(ob, "\x1B[0m    \x1B[47;30m");
			bufput(ob, code.c_str() + lastPos, findPos - lastPos);
			if (findPos - lastPos < windowWidth - 4)
			{
				bufputcn(ob, ' ', windowWidth - 4 - findPos + lastPos);
				if (nl)
				{
					bufputc(ob, '\n');
				}
			}
			else
			{
				bufputc(ob, '\n');
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
			if (ob->size)
			{
				bufputc(ob, '\n');
			}
			::std::string
				out = cmdmd::CPP(::std::string(text->data, text->size));
			bufput(ob, out.c_str(), out.length());
			bufputc(ob, '\n');
			return;
		}
		else if (!strncmp(name, "pawn", 3))
		{
			if (ob->size)
			{
				bufputc(ob, '\n');
			}
			::std::string
				out = cmdmd::Pawn(::std::string(text->data, text->size));
			bufput(ob, out.c_str(), out.length());
			bufputc(ob, '\n');
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
			bufputc(ob, '\n');
		}
		size_t
			width = reinterpret_cast<console_data_s *>(opaque)->WindowWidth,
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
			BUFPUTSL(ob, " \x1B[0;49m\n\x1B[42;5;30m");
			bufputcn(ob, '=', len + 2);
			BUFPUTSL(ob, "\x1B[0m\n\x1B[0m");
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
			bufputc(ob, '\n');
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
				bufputc(ob, '\n');
			}
			else
			{
				BUFPUTSL(ob, " \x1B[35m*\x1B[0m ");
				bufput(ob, text->data, text->size);
				bufputc(ob, '\n');
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
		size_t
			width = reinterpret_cast<console_data_s *>(opaque)->WindowWidth,
			rem = width,
			len = text->size;
		bool
			nl = reinterpret_cast<console_data_s *>(opaque)->BufferWidth > width;
		char
			* start = text->data,
			* end;
		while ((end = (char *)memchr(start, '\n', len)))
		{
			size_t
				diff = end - start;
			len -= diff + 1;
			while (diff >= rem)
			{
				bufput(ob, start, rem);
				diff -= rem;
				start += rem;
				rem = width;
				if (nl)
				{
					bufputc(ob, '\n');
				}
			}
			if (diff)
			{
				bufput(ob, start, diff);
				rem = width - diff;
			}
			start = end + 1;
		}
		if (len)
		{
			while (len >= rem)
			{
				bufput(ob, start, rem);
				len -= rem;
				start += rem;
				rem = width;
				if (nl)
				{
					bufputc(ob, '\n');
				}
			}
			if (len)
			{
				bufput(ob, start, len);
			}
		}
	}
	bufputc(ob, '\n');
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
	bufputc(ob, '\n');
	return 1;
}

void rndr_hrule(struct buf *ob, void *opaque)
{
	if (ob->size)
	{
		bufputc(ob, '\n');
	}
	size_t
		len = reinterpret_cast<console_data_s *>(opaque)->WindowWidth;
	BUFPUTSL(ob, "\x1B[44;34m");
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
		windowWidth = 80,
		bufferWidth = 80;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console))
	{
		// Use the console window size, not the line buffer size.
		windowWidth = console.srWindow.Right - console.srWindow.Left + 1;
		bufferWidth = console.dwSize.X;
	}
	console_data_s
		data = { windowWidth, bufferWidth, 0 };
#else
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	console_data_s
		data = { w.ws_col, w.ws_col, 0 };
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

::std::string
	cmdmd::
	Literals::
	operator "" _md(char const * s, size_t len)
{
	return ::cmdmd::Render(s, len);
}

