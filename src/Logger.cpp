#include "Logger.h"

#ifdef _WIN32
#include <stdio.h>
#include "Console.h"
#elif defined(ANDROID)
#include <android/log.h>
#endif
#ifdef DESKTOP
#include <glew.h>
#else
#include <GLES2/gl2.h>
#endif

#include "Scenes/SceneManager.h"
#include "Helpers/Helpers.h"

#include "Context.h"
#include "TimeManager.h"
#include "StarEngine.h"

namespace star
{
	Logger::Logger()
		:Singleton<Logger>()
#ifdef _WIN32
		,m_ConsoleHandle(nullptr)
		,m_UseConsole(false)
		,m_LogStream()
#else
		,m_LogStream()
#endif
		,m_TimeStamp(_T("00:00:00"))
	{

	}
	
	Logger::~Logger()
	{
		SaveLogFile();
#ifdef _WIN32
		if(m_UseConsole)
		{
			star_w::CleanUpConsole();
		}
#endif
	}

#ifdef _WIN32
	void Logger::Initialize(bool useConsole)
	{
		m_UseConsole = useConsole;
		if(useConsole)
		{
			star_w::RedirectIOToConsole();
			m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		}
#ifndef NO_LOG_FILE
		InitializeLogStream();
#endif
	}
#else
	void Logger::Initialize()
	{
#ifndef NO_LOG_FILE
		InitializeLogStream();
#endif
	}
#endif
	
	void Logger::Update(const Context& context)
	{
		m_TimeStamp = context.time->GetTimeStamp();
	}

	void Logger::Log(
		LogLevel level,
		const tstring& message,
		const tstring& tag,
		const BreakInformation& breakInfo
		)
	{
		tstring levelName;
		switch(level)
		{
		case LogLevel::Info :
			levelName = _T("INFO");
			break;
		case LogLevel::Warning:
			levelName = _T("WARNING");
			break;
		case LogLevel::Error:
			levelName = _T("ERROR");
			break;
		case LogLevel::Debug:
			levelName = _T("DEBUG");
			break;
		}

		PrivateLog(level, message, tag, levelName, breakInfo);
	}

	void Logger::Log(
		LogLevel level,
		const tstring& message,
		const BreakInformation& breakInfo
		)
	{
		Log(level, message, GAME_LOG_TAG, breakInfo);
	}

	void Logger::DebugLog(
		LogLevel level,
		const tstring& message,
		const tstring& tag,
		const BreakInformation& breakInfo
		)
	{
	#ifdef _DEBUG
		tstring levelName;
		switch(level)
		{
		case LogLevel::Info :
			levelName = _T("INFO-D");
			break;
		case LogLevel::Warning:
			levelName = _T("WARNING-D");
			break;
		case LogLevel::Error:
			levelName = _T("ERROR-D");
			break;
		case LogLevel::Debug:
			levelName = _T("DEBUG");
			break;
		}

		PrivateLog(level, message, tag, levelName, breakInfo);
	#endif
	}

	void Logger::DebugLog(
		LogLevel level,
		const tstring& message,
		const BreakInformation& breakInfo
		)
	{
	#ifdef _DEBUG
		DebugLog(level, message, GAME_LOG_TAG, breakInfo);
	#endif
	}

	void Logger::DebugLog(
		const tstring& message,
		const tstring& tag,
		const BreakInformation& breakInfo
		)
	{
	#ifdef _DEBUG
		DebugLog(LogLevel::Debug, message, tag, breakInfo);
	#endif
	}

	void Logger::DebugLog(
		const tstring& message,
		const BreakInformation& breakInfo
		)
	{
	#ifdef _DEBUG
		DebugLog(message, GAME_LOG_TAG, breakInfo);
	#endif
	}

	void Logger::OpenGLLog(const BreakInformation& breakInfo) 
	{
#if LOGGER_MIN_LEVEL > 0
		GLenum err(glGetError());
		while(err != GL_NO_ERROR) 
		{
			tstring error;
			switch(err) 
			{
				case GL_INVALID_OPERATION:      
					error = _T("INVALID_OPERATION");     
					break;
				case GL_INVALID_ENUM:
					error = _T("INVALID_ENUM");
					break;
				case GL_INVALID_VALUE:
					error = _T("INVALID_VALUE");
					break;
				case GL_OUT_OF_MEMORY:  
					error = _T("OUT_OF_MEMORY"); 
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					error = _T("INVALID_FRAMEBUFFER_OPERATION");
					break;
				default:
					error =_T("UNKNOWN_ERROR");
					break;
			}
			tstringstream buffer;
			buffer << "GL_" << error
				   << " - " << breakInfo.file << ":"
				   << breakInfo.line;
#ifndef NO_LOG_FILE
			LogMessage(buffer.str());
#endif
			Log(LogLevel::Error,
				buffer.str(), _T("OPENGL"), breakInfo);
			err = glGetError();
		}
#endif
	}

	void Logger::SetLogSaveDelayTime(float32 seconds)
	{
#ifndef NO_LOG_FILE
		SceneManager::GetInstance()->GetTimerManager()->SetTargetTimeTimer(
			_T("STAR_LogSaveFileTimer"), seconds, true, false);
		SaveLogFile();
#endif
	}
	
	void Logger::PrivateLog(
		LogLevel level,
		const tstring& message,
		const tstring& tag,
		const tstring& levelName,
		const BreakInformation& breakInfo
		)
	{
#if LOGGER_MIN_LEVEL > 0
	#ifdef DESKTOP
		tstringstream messageBuffer;
		messageBuffer << _T("[") << tag
					  << _T("] ") << _T("[")
					  << levelName <<  _T("] ")
					  << message;
		if(breakInfo.line != -1 && tag != STARENGINE_LOG_TAG)
		{
			messageBuffer << _T(" (L")
						  << string_cast<tstring>(breakInfo.line)
						  << _T("@") << breakInfo.file
						  << _T(")");
		}
		messageBuffer << std::endl;
		tstring combinedMessage = messageBuffer.str();
		
		if(m_UseConsole)
		{
			switch(level)
			{
			case LogLevel::Info :
				#if LOGGER_MIN_LEVEL < 2
				SetConsoleTextAttribute(
					m_ConsoleHandle,
					FOREGROUND_INTENSITY |
						FOREGROUND_RED |
						FOREGROUND_GREEN |
						FOREGROUND_BLUE
					);
				#endif
				break;
			case LogLevel::Warning :
				#if LOGGER_MIN_LEVEL < 3
				SetConsoleTextAttribute(
					m_ConsoleHandle,
					FOREGROUND_INTENSITY |
						FOREGROUND_RED |
						FOREGROUND_GREEN
					);
				#endif
				break;
			case LogLevel::Error :
				#if LOGGER_MIN_LEVEL < 4
				SetConsoleTextAttribute(
					m_ConsoleHandle,
					FOREGROUND_INTENSITY |
						FOREGROUND_RED
					);
				#endif
				break;
			case LogLevel::Debug :
				#if LOGGER_MIN_LEVEL < 5
				#ifdef _DEBUG
				SetConsoleTextAttribute(
					m_ConsoleHandle,
					FOREGROUND_INTENSITY |
						FOREGROUND_GREEN
					);
				#endif
				#endif
				break;
			}
			tprintf(combinedMessage.c_str());
		}
		else
		{
			OutputDebugString(combinedMessage.c_str());
		}
		#ifndef NO_LOG_FILE
		LogMessage(combinedMessage);
		#endif
	#else
		switch(level)
		{
		case LogLevel::Info:
			#if LOGGER_MIN_LEVEL < 2
			__android_log_print(
				ANDROID_LOG_INFO,
				tag.c_str(), "%s",
				message.c_str()
				);
			#endif
			break;
		case LogLevel::Warning:
			#if LOGGER_MIN_LEVEL < 3
			__android_log_print(
				ANDROID_LOG_WARN,
				tag.c_str(), "%s",
				message.c_str()
				);
			#endif
			break;
		case LogLevel::Error:
			#if LOGGER_MIN_LEVEL < 4
			__android_log_print(
				ANDROID_LOG_ERROR,
				tag.c_str(), "%s",
				message.c_str()
				);
			#endif
			break;
		case LogLevel::Debug:
			#if LOGGER_MIN_LEVEL < 5
			#ifdef DEBUG
			__android_log_print(
				ANDROID_LOG_DEBUG,
				tag.c_str(),
				message.c_str()
				);
			#endif
			#endif
			break;
		}
		#ifndef NO_LOG_FILE
		tstringstream messageBuffer;
		messageBuffer << _T("[") << tag
					  << _T("] ") << _T("[")
					  << levelName <<  _T("] ")
					  << message << std::endl;
		LogMessage(messageBuffer.str());
		#endif
	#endif
#endif
	}

	void Logger::InitializeLogStream()
	{
		SceneManager::GetInstance()->GetTimerManager()->CreateTimer(
			_T("STAR_LogSaveFileTimer"), 60.0f,
			false, true, [&] () { SaveLogFile(); }, false);

		m_LogStream << _T("++++++++++++++++++++++++++++++++++++++\
++++++++++++++++++++++++++++++++++++++++++++++++++") << std::endl << std::endl;
		m_LogStream << _T("	Star Engine version ")
					<< STARENGINE_VERSION << std::endl
					<< std::endl;
		m_LogStream << _T("	Game is compiled in");

	#ifdef _DEBUG
		m_LogStream << _T(" debug mode.") << std::endl;
	#else
		m_LogStream << _T(" release mode.") << std::endl;
#endif
	#if LOGGER_MIN_LEVEL < 2
		m_LogStream << _T("	All Star::Logging levels are enabled.") << std::endl;
	#elif LOGGER_MIN_LEVEL < 3
		m_LogStream << _T("	Star::Logging level info is disabled.") << std::endl;
	#elif LOGGER_MIN_LEVEL < 4
		m_LogStream << _T("	Star::Logging levels info and warning is disabled.") << std::endl;
	#elif LOGGER_MIN_LEVEL < 5
		m_LogStream << _T("	Star::Logging levels info, warning and error is disabled.") << std::endl;
	#elif LOGGER_MIN_LEVEL < 6
		m_LogStream << _T("	All Star::Logging levels are disabled.") << std::endl;
	#endif
		m_LogStream << std::endl;
		m_LogStream << _T("	The Star Engine is licensed under the MIT License.") << std::endl;
		m_LogStream << _T("	For more information, go to: \
http://www.starengine.eu/") << std::endl << std::endl;
		m_LogStream << _T("++++++++++++++++++++++++++++++++++\
++++++++++++++++++++++++++++++++++++++++++++++++++++++") << std::endl << std::endl;
	}
	
	void Logger::LogMessage(const tstring& message)
	{
		m_LogStream << _T("[") << m_TimeStamp << _T("] ") << message;
	}

	void Logger::SaveLogFile()
	{
		WriteTextFile(_T("StarLog.txt"), m_LogStream.str(), DirectoryMode::internal);
	}
}
