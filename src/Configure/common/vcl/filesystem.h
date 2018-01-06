//---------------------------------------------------------------------------

#ifndef filesystemH
#define filesystemH

#include <string>
#include <vector>
using namespace std;




class FileSystem {
public:
	static vector<string>getFileList( string path,
				const vector<string> &extensions, bool addPathToResult,
				int fromFrame = -1, int toFrame = -1 );
	//упорядоченный по номерам

	static string pathNormalize( string path );		 //добавить '\\' в конце

    static string extractPath( const string &fileName );    //выделяет путь до файла

};

//---------------------------------------------------------------------------
#endif
