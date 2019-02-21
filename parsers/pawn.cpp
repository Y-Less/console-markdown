#include <string>
#include <string_view>
#include <sstream>
#include <string.h>
#include <regex>
#include "../include/console-markdown/Pawn.hpp"
#include "../StringUtils.hpp"

// This parser is NOT comprehensive.  If you do silly things like escaped quotes with comments in
// strings, it will probably break...  That isn't the point - it's just a simple debugging aid.
static ::std::regex
	sMatchers[] = {
		::std::regex("/\\*(.|\\r|\\n)*?\\*/"),
		::std::regex("//.*"),
		// Can do mutli-line pre-processor directives (ones with `\` at the end).
		::std::regex("^\\s*#(.*?\\\\\\n)*.*"),
		::std::regex("(!|!\\\\|\\\\!|\\\\)?\"(\"|.*?[^\\\\]\")|\\'\\\\?.\\'"),
		// Will detect most hex, binary, decimal, and floats.  Won't detect complex things like
		// `.5e5`.  Again, this is for SIMPLE examples...
		::std::regex("\\b(0x[0-9a-fA-F]+|0b[01]+|\\-?[0-9]+(\\.[0-9]+)?)\\b"),
		// Now colours multiple symbols separated by spaces with the same code, just for efficiency.
		::std::regex("([\\!\\%\\^\\&\\*\\(\\)\\-\\=\\+\\[\\]\\{\\}\\~\\,\\<\\>\\/\\?\\|\\;\\:\\#]+[\\s\\r\\n]*)+"),
		::std::regex("\\b(true|false|public|forward|native|const|static|stock|assert|break|case|continue|default|do|else|for|goto|if|return|sleep|state|switch|while|char|enum|new)\\b"),
		::std::regex("\\b(hook|inline|foreign|global|timer|remotefunc|iterfunc|uvar|svar|__declspec|master_hook|master_task|master_ptask|group_hook|master_func|task|ptask|@foreign|@global|mhook|foreach|defer|repeat|stop|call|using|loadtext|yield|localfunc|broadcastfunc|targetfunc|final)\\b"),
	};

static char const * const
	sColours[] = {
		"\x1B[32m", // Line comment.
		"\x1B[32m", // Block comment.
		"\x1B[35m", // Pre-processor.
		"\x1B[31;1m", // Strings.
		"\x1B[35;1m", // Numbers.
		"\x1B[30;1m", // Symbols.
		"\x1B[36;1m", // Keyword.
		"\x1B[36;1m", // YSI.
	};

static char const * const
	sDefault = "\x1B[37;1m";

static size_t
	gYSI;

static void
	DoRegex(::std::stringstream & highlighted, ::std::string::const_iterator start, ::std::string::const_iterator end, size_t idx)
{
	if (start == end)
		return;
	if (idx == gYSI)
	{
		// Out of regexes to check.
		highlighted << sDefault << ::std::string_view(&*start, ::std::distance(start, end));
		return;
	}
	::std::smatch m;
	while (::std::regex_search(start, end, m, sMatchers[idx]))
	{
		size_t
			pos = m.position(),
			len = m.length();
		::std::string::const_iterator
			mid = start + pos;
		DoRegex(highlighted, start, mid, idx + 1);
		highlighted << sColours[idx] << ::std::string_view(&*mid, len);
		start = mid + len;
	}
	DoRegex(highlighted, start, end, idx + 1);
}

::std::string
	cmdmd::
	Pawn(::std::string const & s, bool ysi)
{
	gYSI = ysi ? 8 : 7;
	::std::stringstream
		highlighted;
	highlighted << "    ";
	DoRegex(highlighted, s.begin(), s.end(), 0);
	::std::string
		ret = highlighted.str();
	ReplaceAll(ret, "\t", "    ");
	ReplaceAll(ret, "\n", "\n    ");
	// Reset the colour.
	return ret + "\x1B[0m";
}

::std::string
	cmdmd::
	Literals::
	operator "" _pawn_syntax(char const * s, size_t len)
{
	return Pawn(::std::string(s, len));
}

