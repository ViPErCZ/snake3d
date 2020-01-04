#pragma once
#include <string>

using namespace std;

namespace fTools {
	class fString :
		public string
	{
	public:
		fString(void);
		fString(const char * str);

		~fString(void);

		// get Funkce
		const char * getString();

		//Replace Funkce
		const char* replace(const char* a_find, const char* a_replace);
		const char* replace(const char* a_find, const char* a_replace, int & a_count);
	};
}