//---------------------------------------------------------------------------


#pragma hdrstop

#include "parsing.h"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//разбить строку на подстроки, используя разделитель delim
vector<string> Parsing::split(const string &str, const string &delim)
{
    vector<string> result;
    int n = str.length();
    int nD = delim.length();
    int i = 0;
    while (i < n) {
        int j = str.find(delim, i);
        if (j >= 0 && j < n) {
            string s = str.substr(i, j - i);
            result.push_back(s);
            i = j + nD;
        } else {
            break;
        }
    }
    if (i < n) {
        result.push_back(str.substr(i));
    }

    return result;

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
