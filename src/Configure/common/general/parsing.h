//---------------------------------------------------------------------------

#ifndef parsingH
#define parsingH

#include <vector>
#include <string>
using namespace std;

//������ �������� �����

class Parsing
{
public:

    //������� ������ �� ���������, ��������� ����������� delim
    static vector<string> split(const string &str, const string &delim);


};

//---------------------------------------------------------------------------
#endif
