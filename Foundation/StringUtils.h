// Copyright (C) 2005-2008 Washington University in St Louis, Baylor College of Medicine.  All rights reserved
// Author:        Sasakthi S. Abeysinghe (sasakthi@gmail.com)
// Description:   A class for string manipulation

// CVS Meta Information: 
//   $Source: /project/mm/cvs/graphics/ssa1/source/Foundation/StringUtils.h,v $
//   $Revision: 1.9 $
//   $Date: 2010/02/11 23:19:11 $
//   $Author: ssa1 $
//   $State: Exp $
//
// History Log: 
//   $Log: StringUtils.h,v $
//   Revision 1.9  2010/02/11 23:19:11  ssa1
//   Allowing the ability to save pseudoatoms generated from SSEHunter
//
//   Revision 1.8  2008/09/29 19:05:37  ssa1
//   Fixing String Utils.h linking errors
//
//   Revision 1.7  2008/09/29 16:22:17  ssa1
//   Removing compiler warnings
//
//   Revision 1.6  2008/09/29 15:45:09  ssa1
//   Adding in CVS meta information
//

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
			static string DoubleToString(double number, int padSize = -1, string padChar = "0", int digits = -1, int decimals = -1);
			static string IntToString(int number, int padSize = -1, string padChar = "0");
			static string CharToString(char c);
			static double StringToDouble(string s);
			static int StringToInt(const string &s);
			static string StringToUpper(string strToConvert);
			static string StringToLower(string strToConvert);
			static void RightTrim(string &source, string t);
			static void LeftTrim(string &source, string t);
			static string RightPad(string source, int strLength, string padChar = " ");
			static string LeftPad(string source, int strLength, string padChar = " ");
		};	
		
		string StringUtils::DoubleToString(double number, int padSize, string padChar, int digits, int decimals) {
			char * x = new char[20];
			char * y = new char[20];
			if((digits > 0) && (decimals > 0) ) {
				sprintf(y, "%%%d.%df", digits, decimals);
				sprintf(x, y, number);
			} else {
				sprintf(x, "%f", number);
			}
			string retVal = x;

			int start = retVal.size();
			for(int i = start; i <= padSize; i++) {
				retVal = padChar + retVal;
			}

			delete [] x;
			return retVal;
		}

		string StringUtils::IntToString(int number, int padSize, string padChar) {
			char * x = new char[20];
			sprintf(x, "%d", number);
			string retVal = x;
			int start = retVal.size();
			for(int i = start; i <= padSize; i++) {
				retVal = padChar + retVal;
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

		string StringUtils::StringToUpper(string strToConvert) {
		   for(unsigned int i=0;i<strToConvert.length();i++)   {
		      strToConvert[i] = (char)toupper(strToConvert[i]);
		   }
		   return strToConvert;
		}

		string StringUtils::StringToLower(string strToConvert)
		{
		   for(unsigned int i=0;i<strToConvert.length();i++)  {
		      strToConvert[i] = (char)tolower(strToConvert[i]);
		   }
		   return strToConvert;
		}			

		void StringUtils::RightTrim(string &source, string t) {
			source.erase(source.find_last_not_of(t)+1);
		}

		void StringUtils::LeftTrim(string &source, string t) {
			source.erase(0, source.find_first_not_of(t));
		}			

		string StringUtils::RightPad(string source, int strLength, string padChar) {
			string temp = "";
			int i;
			for(i = 0; i < min((int)source.size(), strLength); i++) {
				temp.push_back(source[i]);
			}
			for(int j = i; j < strLength; j++) {
				temp = temp + padChar;
			}
			return temp;
		}

		string StringUtils::LeftPad(string source, int strLength, string padChar) {
			string temp = "";
			int i;
			for(int j = 0; j < strLength - (int)source.size(); j++) {
				temp = padChar + temp;
			}

			for(i = 0; i < min((int)source.size(), strLength); i++) {
				temp.push_back(source[i]);
			}
			return temp;
		}
	}
}

#endif