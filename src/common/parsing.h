//---------------------------------------------------------------------------

#ifndef parsingH
#define parsingH

#include <vector>
#include <string>
using namespace std;

#include "converter.h"

//������ �������� ����� 

class Parsing {
public:

	//������� ������ �� ���������, ��������� ����������� delim
	static vector<string> split( const string &str, const string &delim );
	
	//��������� ������� � ���������
	static vector<string> splitFiltered( const string &str, const string &delim );
	static vector<float> splitFilteredFloat( const string &str, const string &delim );
	static vector<int> splitFilteredInt( const string &str, const string &delim );

	//��������� ������� � ���������
	static vector<float> splitToFloats( const string &str, const string &delim );

};

//---------------------------------------------------------------------------
#endif
