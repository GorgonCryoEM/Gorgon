#ifndef FOUNDATION_STRING_UTILS_H
#define FOUNDATION_STRING_UTILS_H

#include <string>
#include <sstream> 
#include <cctype>

using namespace std;

namespace wustl_mm {
	namespace Foundation {
		class StringUtils {
		public:
			static string DoubleToString(double number);
			static string IntToString(int number, int padSize = -1);
			static string CharToString(char c);
			static double StringToDouble(string s);
			static int StringToInt(const string &s);
			static string StringToUpper(string strToConvert) {
			   for(unsigned int i=0;i<strToConvert.length();i++)   {
			      strToConvert[i] = toupper(strToConvert[i]);
			   }
			   return strToConvert;
			}
			static string StringToLower(string strToConvert);
			static void RightTrim(string &source, string &t);
			static void LeftTrim(string &source, string &t);
		};	
		
		string StringUtils::DoubleToString(double number) {
			char * x = new char[20];
			sprintf(x, "%f", number);
			string retVal = x;
			delete [] x;
			return retVal;
		}

		string StringUtils::IntToString(int number, int padSize) {
			char * x = new char[20];
			sprintf(x, "%d", number);
			string retVal = x;
			int start = retVal.size();
			for(int i = start; i <= padSize; i++) {
				retVal = "0" + retVal;
			}
			delete [] x;
			return retVal;
		}

		string StringUtils::CharToString(char c) {
			char * x = new char[2];
			x[0] = c;
			x[1] = 0;
			string retval = string(x);
			delete [] x;
			return retval;
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
		string StringToLower(string strToConvert)
		{
		   for(unsigned int i=0;i<strToConvert.length();i++)  {
		      strToConvert[i] = tolower(strToConvert[i]);
		   }
		   return strToConvert;
		}			

		void RightTrim(string &source, string &t) {
			source.erase(source.find_last_not_of(t)+1);
		}

		void LeftTrim(string &source, string &t) {
			source.erase(0, source.find_first_not_of(t));
		}			
	}
}

#endif