//---------------------------------------------------------------------------

#ifndef docutilsH
#define docutilsH

#include<string>
using namespace std;

//�������� ����������, �������

class DocUtils {
public:
	static void browsePage( const string &url );   	//�������� � email
	static void openFile( const string &fileName );

private:
	static void openDoc( const string &url );

};


//---------------------------------------------------------------------------
#endif
