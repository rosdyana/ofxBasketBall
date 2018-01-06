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
	//������������� �� �������

	static string pathNormalize( string path );		 //�������� '\\' � �����

    static string extractPath( const string &fileName );    //�������� ���� �� �����

};

//---------------------------------------------------------------------------
#endif
