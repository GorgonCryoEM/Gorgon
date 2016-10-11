#ifndef TOOLKIT_FOUNDATION_STRING_UTILS_H
#define TOOLKIT_FOUNDATION_STRING_UTILS_H

#include <string>
#include <sstream>
#include <cctype>
#include <algorithm>

using namespace std;

namespace Foundation {
    class StringUtils {
    public:
        static string DoubleToString(double number, int padSize = -1, string padChar = "0", int digits = -1, int decimals = -1);
        static string IntToString(int number, int padSize = -1, string padChar = "0");
        static string StringToUpper(string strToConvert);
        static string StringToLower(string strToConvert);
        static void RightTrim(string &source, string t);
        static void LeftTrim(string &source, string t);
        static string RightPad(string source, int strLength, string padChar = " ");
        static string LeftPad(string source, int strLength, string padChar = " ");
    };

    inline string StringUtils::DoubleToString(double number, int padSize, string padChar, int digits, int decimals) {
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

    inline string StringUtils::IntToString(int number, int padSize, string padChar) {
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

    inline string StringUtils::StringToUpper(string strToConvert) {
       for(unsigned int i=0;i<strToConvert.length();i++)   {
          strToConvert[i] = (char)::toupper(strToConvert[i]);
       }
       return strToConvert;
    }

    inline string StringUtils::StringToLower(string strToConvert)
    {
       for(unsigned int i=0;i<strToConvert.length();i++)  {
          strToConvert[i] = (char)::tolower(strToConvert[i]);
       }
       return strToConvert;
    }

    inline void StringUtils::RightTrim(string &source, string t) {
        source.erase(source.find_last_not_of(t)+1);
    }

    inline void StringUtils::LeftTrim(string &source, string t) {
        source.erase(0, source.find_first_not_of(t));
    }

    inline string StringUtils::RightPad(string source, int strLength, string padChar) {
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

    inline string StringUtils::LeftPad(string source, int strLength, string padChar) {
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

#endif
