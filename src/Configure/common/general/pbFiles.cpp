#include "pbFiles.h"


//--------------------------------------------------------------------------------
bool pbFiles::fileExists( const string &fileName )
{
	ifstream inp;
	inp.open(fileName.c_str(), ifstream::in);
	inp.close();
	return !inp.fail();
}

//--------------------------------------------------------------------------------
/*int pbFiles::fileSize(  const string &fileName )
{
	FILE *file = NULL;
	fopen( &file, fileName.c_str(), "rb" );
	fseek( file, 0, SEEK_END );
	int size = ftell( file );	//TODO size_t גלוסעמ int
	fclose( file );
	return size;
}*/

//--------------------------------------------------------------------------------
vector<string> pbFiles::readStrings( const string &fileName )
{
	vector<string> list;
	if ( fileExists( fileName ) ) {
		ifstream f(fileName.c_str(),ios::in);
		string line;
		while (getline(f,line)) {
			list.push_back( line );
		}
	}                              
	return list;                   
}

//--------------------------------------------------------------------------------

