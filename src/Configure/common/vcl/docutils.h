//---------------------------------------------------------------------------

#ifndef docutilsH
#define docutilsH

#include<string>
using namespace std;

//Открытие документов, страниц

class DocUtils {
public:
	static void browsePage( const string &url );   	//понимает и email
	static void openFile( const string &fileName );

private:
	static void openDoc( const string &url );

};


//---------------------------------------------------------------------------
#endif
