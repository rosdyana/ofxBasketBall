//---------------------------------------------------------------------------

#ifndef converterH
#define converterH

#include <string>
#include <sstream>
#include <vector>
#include <windows.h>		//�� �����-������������!

using namespace std;

class Converter {
public:
	static int strToInt(const string& s)
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
		int d = len - s.length();		//DUMMY ��� ����� �������� ������
		for (int i=0; i<d; i++) {
				s = '0' + s;
		}
	   }
	   return s;
	}

    static double strToDouble(const string& s)
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

	static vector<string> vectorFloatToString( const vector<float> &v ){
		vector<string> sv( v.size() );
		for (int i=0; i<v.size(); i++) {
			sv[ i ] = doubleToStr( v[ i ] );
		}
		return sv;
	}
	static vector<float> vectorStringToFloat( const vector<string> &sv ){
		vector<float> v( sv.size() );
		for (int i=0; i<sv.size(); i++) {
			v[ i ] = strToDouble( sv[ i ] );
		}
		return v;
	}

	//����������� ������ � wstr, ��� - ������������ ��������� �� ���������
	static wstring strToWstr( const string &text )
	{
				// ����� ��� ��������� ����������
		WCHAR buffer[256];

		// �������������� ������ � Unicode
		MultiByteToWideChar(CP_ACP, 0, text.c_str(), strlen(text.c_str()) + 1, buffer, sizeof(buffer)/sizeof(WCHAR));
		return buffer;
	}

	//������ ���� ��������� ������ �� ������
	static string replaceAll(const string& context0, const string& from, const string& to)
	{
		string context = context0;
		size_t lookHere = 0;
		size_t foundHere;
		while((foundHere = context.find(from, lookHere)) != string::npos)
		{
			context.replace(foundHere, from.size(), to);
			lookHere = foundHere + to.size();
		}
		return context;
	}

	//������������� \t � �������, ������� ��������� �������� � ������� �� �� ������ � �����
	static string filterString( string s )
	{
		s = replaceAll( s, "\t", " ");
		string res = "";
		int i = 0;
		while ( i < s.length() ) {
			if ( s[i] != ' ' || s[i] == ' ' && i > 0 && s[i-1] != ' ' ) {
				res += s[i];
			}
			i++;
		}
		//������� ������ � ������ � �����
		if ( res.length() > 0 && res[0] == ' ' ) res = res.substr( 1 );
		if ( res.length() > 0 && res[res.length() - 1] == ' ' ) res = res.substr( 0, res.length() - 1 );
		return res;
	}


};

//---------------------------------------------------------------------------
#endif
