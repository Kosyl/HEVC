#include "TypeDef.h"

template<typename T>
Void Logger::write(T s, identity<T> i) 
{
	if(m_printSpaces)
	{
		printSpaces();
	}

	m_ss << s;
}

Void write(const Char* s, identity<const Char*> i)
{
	std::string str(s);
	write(str);
}

Void write(std::string s, identity<std::string> i)
{
	if(s=="\n" || s.find("\n")!=std::string::npos)
	{
		m_printSpaces=true;
		m_ss << s;
		emptyBuffer();
		return;
	}
	else
	{
		if(m_printSpaces)
		{
			printSpaces();
		}
		m_ss << s;
	}
}