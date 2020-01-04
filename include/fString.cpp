#include <cstring>
#include "fString.h"
using namespace fTools;

fString::fString(void)
{
}

fString::fString(const char * str)
:string(str)
{
}

fString::~fString(void)
{
}

const char * fString::getString() {
	return string::data();
}

const char* fString::replace(const char* a_find, const char* a_replace)
{
 	int a_pos = 0;

 	while ((a_pos = (int)string::find(a_find, a_pos)) != string::npos)
	{
		string::replace(a_pos, strlen(a_find), a_replace);
		a_pos = 0;
	}

	return string::data();
}

const char* fString::replace(const char* a_find, const char* a_replace, int & a_count)
{
 	int a_pos = 0;
	a_count = 0;

	while ((a_pos = (int)string::find(a_find, a_pos)) != -1)
	{
		string::replace(a_pos, strlen(a_find), a_replace);
		a_pos = 0;
		a_count++;
	}

	return string::data();
}