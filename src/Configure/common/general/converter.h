//---------------------------------------------------------------------------

#ifndef converterH
#define converterH

#include <string>
#include <sstream>

using namespace std;

class Converter
{
public:
    static int strToInt(const string &s)
    {
        istringstream i(s);
        int x;
        if (!(i >> x)) {
            x = 0;
        }
        return x;
    }

    static string intToStr(int x, int len = 0)
    {
        ostringstream o;
        if (!(o << x)) {
        }
        string s = o.str();
        if (len > 0) {
            int d = len - s.length();		//DUMMY это можно написать короче
            for (int i = 0; i < d; i++) {
                s = '0' + s;
            }
        }
        return s;
    }

    static double strToDouble(const string &s)
    {
        istringstream i(s);
        double x;
        if (!(i >> x)) {
            x = 0;
        }
        return x;
    }

    static string doubleToStr(double x, int len = 0)
    {
        ostringstream o;
        if (!(o << x)) {
        }
        string s = o.str();
        return s;
    }
};

//---------------------------------------------------------------------------
#endif
