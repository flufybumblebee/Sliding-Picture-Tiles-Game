#pragma once

#include <string>
#include <sstream>
#include <debugapi.h>

namespace Testing
{
	void OutputToDebug( const std::wstring& WSTRING )
	{
		OutputDebugString( WSTRING.c_str() );
	}
	void OutputToDebug( const std::wostringstream& WOSSTREAM )
	{
		OutputDebugString( WOSSTREAM.str().c_str() );
	}
	void OutputToDebug( const std::string& STRING )
	{
		OutputDebugStringA( STRING.c_str() );
	}
	void OutputToDebug( const std::ostringstream& OSSTREAM )
	{
		OutputDebugStringA( OSSTREAM.str().c_str() );
	}
}
