#include "Logger.h"
/** \file     Logger.cpp
\brief    Klasa dajaca klasom pochodnym narzedzia logowania
*/

Logger::Logger()
{
	Logger("D:\\txt\\default_log.txt",false);
}

/** Konstruktor przyjmuje sciezke do logu i rozkaz wlaczenia/nie wlaczania logu
*	\param logPath sciezka do pliku logu
\param isLogging pisze log, jesli true
*/
Logger::Logger(std::string logPath, Bool isLogging):
	includeDetails(true),
	m_loggingOn(isLogging),
	m_printSpaces(true),
	m_logPath(logPath),
	m_numTabs(0),
	m_step(4),
	m_spaces("")
{
	if(isLogging)
	{
		m_logStream.open(logPath,std::fstream::out | std::fstream::ate);
	}
}

Logger::~Logger()
{
	if(m_logStream.is_open())
	{
		m_logStream.flush();
		m_logStream.close();
	}
}

/** Wlaczenie/wylaczenie logowania
*	\param log true, jesli ma logowac; false, jesli nie
*	\return void  
*/

Void Logger::printSpaces()
{
	m_printSpaces=false;
	m_logStream << m_spaces;
}

void Logger::setLogging(Bool log)
{
	if(log)
	{
		if(!m_logStream.is_open())
		{
			m_logStream.open(m_logPath,std::fstream::out | std::fstream::ate);
		}
	}
	else
	{
		m_logStream.flush();
		m_logStream.close();
	}
}

Void Logger::changeLogPath(std::string path)
{
	if(m_logStream.is_open())
	{
		m_logStream.flush();
		m_logStream.close();
	}

	m_logPath=path;
	m_logStream.open(path, std::fstream::out | std::fstream::ate);

	return;
}

Void Logger::setTabLength(UInt len)
{
	if(len>=0)
	{
		m_spaces = "";
		for(int i = 0; i<len*m_step;++i)
		{
			m_spaces = m_spaces.append(" ");
		}

		m_numTabs = len;
	}
	else
		setTabLength(0);
}

Void Logger::setTabStep(UInt len)
{
	m_step = len;
	setTabLength(m_numTabs);
}

Void Logger::increaseSpaces()
{
	setTabLength(m_numTabs+1);
}

Void Logger::decreaseSpaces()
{
	setTabLength(m_numTabs-1);
}

Logger& operator<<(Logger& o, std::ostream& (*f)(std::ostream&))
{
	if(o.isLogging())
	{
		o.write("\n");
	}
	return o;
}