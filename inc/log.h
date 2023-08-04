#ifndef ARPCTL_LOG_H
#define ARPCTL_LOG_H

#include "framework.h"

constexpr std::string_view CRIT = "crit";
constexpr std::string_view FAIL = "fail";
constexpr std::string_view WARN = "warn";
constexpr std::string_view INFO = "info";
constexpr std::string_view NOTE = "note";
constexpr std::string_view VERB = "verb";

template<char... str>
struct char_seq final
{
	static constexpr const char data[] = { str..., 0 };
};

class msg_body final
{
private:
	std::basic_ostream<char>& _out;
	std::stringstream _buf;

public:
	explicit msg_body(std::string_view heading, std::basic_ostream<char>& out);

	~msg_body();

public:
	template<typename T>
	msg_body& operator<<(const T& arg)
	{
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

#endif //ARPCTL_LOG_H
