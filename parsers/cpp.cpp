#include <string>
#include <string_view>
#include <sstream>
#include <string.h>
#include <regex>
#include "../include/console-markdown/CPP.hpp"
#include "../StringUtils.hpp"

// This parser is NOT comprehensive.  If you do silly things like escaped quotes with comments in
// strings, it will probably break...  That isn't the point - it's just a simple debugging aid.
static ::std::regex
	sMatchers[] = {
		::std::regex("/\\*(.|\\r|\\n)*?\\*/"),
		::std::regex("//.*"),
		// Can do mutli-line pre-processor directives (ones with `\` at the end).
		::std::regex("^\\s*#(.*?\\\\\\n)*.*"),
		// Now handles multi-line raw strings.
		::std::regex("R\"\\((.|\\r|\\n)*?\\)\"|\"\"|\".*?[^\\\\]\"|\\'\\\\?.\\'"),
		// Will detect most hex, decimal, and floats.  Won't detect complex things like `.5e5` or
		// `0ULL`.  Again, this is for SIMPLE examples...
		::std::regex("\\b(0x[0-9a-fA-F]+|\\-?[0-9]+(\\.[0-9]+f?)?)\\b"),
		// Now colours multiple symbols separated by spaces with the same code, just for efficiency.
		::std::regex("([\\!\\%\\^\\&\\*\\(\\)\\-\\=\\+\\[\\]\\{\\}\\~\\,\\<\\>\\/\\?\\|\\;\\:\\#]+[\\s\\r\\n]*)+"),
		// The last bit detects types of the form `name_t` - the `_t` suffix is technically reserved by
		// the C++ spec.
		::std::regex("\\b(auto|bool|break|case|catch|char|class|const|const_cast|continue|default|delete|do|double|dynamic_cast|else|enum|explicit|extern|false|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|nullptr|operator|override|constexpr|decltype|private|protected|public|reinterpret_cast|return|short|signed|sizeof|static|static_cast|struct|switch|template|this|throw|true|try|typedef|typename|union|unsigned|virtual|void|while|[a-zA-Z_][a-zA-Z_0-9]*_t)\\b"),
		// alignas|alignof|and|and_eq|asm|atomic_cancel|atomic_commit|atomic_noexcept|audit|axiom|bitand|bitor|char16_t|char32_t|compl|concept|constexpr|co_await|co_return|co_yield|export|final|import|module|noexcept|not|not_eq|or|or_eq|reflexpr|register|requires|static_assert|synchronized|thread_local|transaction_safe|transaction_safe_dynamic|typeid|using|volatile|wchar_t|xor|xor_eq
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
	};

static char const * const
	sDefault = "\x1B[37;1m";

static void
	DoRegex(::std::stringstream & highlighted, ::std::string::const_iterator start, ::std::string::const_iterator end, size_t idx)
{
	if (start == end)
		return;
	if (idx == 7)
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
	CPP(::std::string const & s)
{
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
	operator "" _cpp_syntax(char const * s, size_t len)
{
	return CPP(::std::string(s, len));
}

