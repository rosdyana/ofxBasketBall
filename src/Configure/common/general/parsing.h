//---------------------------------------------------------------------------

#ifndef parsingH
#define parsingH

#include <vector>
#include <string>
using namespace std;

//Модуль парсинга строк

class Parsing
{
public:

    //разбить строку на подстроки, используя разделитель delim
    static vector<string> split(const string &str, const string &delim);


};

//---------------------------------------------------------------------------
#endif
