#ifndef FOUNDATION_STRING_UTILS_H
#define FOUNDATION_STRING_UTILS_H

#include <string>
#include <sstream> 

using namespace std;

namespace wustl_mm {
	namespace Foundation {
		class StringUtils {
		public:
			static string DoubleToString(double number);
			static double StringToDouble(string s);
			static int StringToInt(const string &s);
		};

		string StringUtils::DoubleToString(double number) {
			char * x = new char[20];
			sprintf(x, "%f", number);
			string retVal = x;
			delete [] x;
			return retVal;
		}

		double StringUtils::StringToDouble(string s) {
			float retVal = 1;
			sscanf(s.c_str(), "%f", &retVal);
			return retVal;
		}

		int StringUtils::StringToInt(const string &s)
		{
		  istringstream myStream(s);
		  int i;
		  
		  myStream>>i;
		  return i;
		}

	}
}

#endif