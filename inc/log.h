#ifndef ARPCTL_LOG_H
#define ARPCTL_LOG_H

#include "framework.h"

#define CRIT "crit"
#define FAIL "fail"
#define WARN "warn"
#define INFO "info"
#define NOTE "note"
#define VERB "verb"

template<char... str>
struct char_seq final
{
	static constexpr const char data[] = { str..., 0 };
};

template<typename T, T... str>
char_seq<str...> operator ""_seq()
{
	return {};
}

class msg_body final
{
private:
	size_t _pad;
	std::basic_ostream<char>& _out;
	std::stringstream _buf;

public:
	msg_body(std::string_view heading, std::basic_ostream<char>& out);

	~msg_body();

public:
	template<typename T>
	msg_body& operator<<(const T& arg)
	{
		_buf << arg;
		return *this;
	}

	msg_body& operator<<(const std::string_view& arg)
	{
		size_t p, o = 0;
		while ((p = arg.find('\n')) != std::string::npos)
		{
			_buf << arg.substr(o, p - o + 1) << std::string(' ', _pad);
			o = p;
		}
		_buf << arg.substr(o, p - o + 1);

		return *this;
	}

	msg_body& operator<<(const char* arg)
	{
		char* window = new char[strlen(arg)];
		char* frame = window;
		strcpy(window, arg);

		char* p;
		while ((p = strchr(window, '\n')))
		{
			*p = 0;
			_buf << window << '\n' << std::string(_pad, ' ');
			window = p + 1;
		}
		_buf << window;

		delete[] frame;

		return *this;
	}

	template<size_t N>
	msg_body& operator<<(const char arg[N])
	{
		char window[N];
		strcpy(window, arg);

		char* p;
		while ((p = strchr(window, '\n')))
		{
			*p = 0;
			_buf << window << '\n' << std::string(_pad, ' ');
			window = p + 1;
		}
		_buf << window;

		return *this;
	}
};

template<typename...>
class msg_head;

template<char... file, char... line, char... head>
class msg_head<char_seq<file...>, char_seq<line...>, char_seq<head...>> final
{
public:
	using file_seq = char_seq<file...>;
	using line_seq = char_seq<line...>;
	using head_seq = char_seq<head...>;

private:
	static consteval std::basic_ostream<char>& buf()
	{
		return __builtin_strcmp(head_seq::data, CRIT) == 0 ||
			   __builtin_strcmp(head_seq::data, FAIL) == 0
			   ? std::cerr
			   : std::cout;
	}

	static constexpr auto heading()
	{
		return std::format("{}:{}: {}: ", file_seq::data, line_seq::data, head_seq::data);
	}

public:
	static msg_body init()
	{
		return { heading(), buf() };
	}
};

#define __TSTR(sym) sym##_seq
#define _TSTR(sym) decltype(__TSTR(sym))
#define __STR(sym) #sym
#define _STR(sym) __STR(sym)

#define LOG(head) msg_head<_TSTR(__FILE_NAME__), _TSTR(_STR(__LINE__)), _TSTR(head)>::init()

#endif //ARPCTL_LOG_H
