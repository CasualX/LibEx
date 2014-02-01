#ifndef HGUARD_LIBEX_TOOLS_ARGUMENTS
#define HGUARD_LIBEX_TOOLS_ARGUMENTS
#pragma once

//----------------------------------------------------------------
// Tools: Arguments
//----------------------------------------------------------------
// Provides command line argument processing

namespace tools
{

class Arguments
{
public:


	// Get the number of arguments
	unsigned arg_count() const;
	// Get a specific argument
	const char* arg( unsigned index ) const;
	// Get the whole command string
	const char* cmd_string() const;
	// Get the remaining arguments starting from
	const char* cmd_string( unsigned index ) const;

private:
	// The whole command string
	const char* _str;
	// Pointers to the start of each argument in the command string
	const char** _tags;
	// Individual arguments buffer
	char* _buf;
	// Pointers to the individual arguments
	char** _args;
	// Number of arguments
	unsigned _num;
};

}

#endif // !HGUARD_LIBEX_TOOLS_ARGUMENTS
