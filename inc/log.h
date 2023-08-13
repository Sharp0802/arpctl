#ifndef ARPCTL_LOG_H
#define ARPCTL_LOG_H

#include "framework.h"
#include "xthread.h"


#ifdef TRACE_FLOW
#define TRACE std::cout << '\n' << reinterpret_cast<size_t>(this) << ":" << __PRETTY_FUNCTION__ << ":" << __FILE_NAME__ << ':' << __LINE__ << '\n'
#define TRACE_S std::cout << '\n' << __PRETTY_FUNCTION__ << ":" << __FILE_NAME__ << ':' << __LINE__ << '\n'
#else
#define TRACE
#define TRACE_S
#endif

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
	std::basic_ostream<char>& _out;
	std::stringstream _buf;

public:
	msg_body(const std::string& heading, std::basic_ostream<char>& out);

	~msg_body();

public:
	template<typename T>
	msg_body& operator<<(const T& arg)
	{
		TRACE;
		_buf << arg;
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

#ifdef TMP_LOGGER
#define LOG(head) msg_head<_TSTR(__FILE_NAME__), _TSTR(_STR(__LINE__)), _TSTR(head)>::init()
#else
#define LOG(head) std::cout << '\n' << '[' << Worker::ID() << ']' << __FILE_NAME__ << ':' << __LINE__ << ": " << head << ": "
#endif

#endif //ARPCTL_LOG_H
