//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "filesystem.h"

#include <ctime>
#include "converter.h"



//---------------------------------------------------------------------------

#pragma package(smart_init)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool isNumber( char c ) {
	return (c >= '0' && c <= '9' );
}

//вз€тие последнего числа в строке дл€ проведени€ сравнени€
int fileNumber( const string &fileName )
{
	int result = -1;
	int i = fileName.length() - 1;
	while ( i >= 0 && !isNumber(fileName[i]) ) {
		i--;
	}
	int j = i-1;
	while ( j >= 0 && isNumber(fileName[j]) ) {
		j--;
	}
	j++;
	i++;
	if ( j >= 0 ) {
		string s = fileName.substr( j, i-j );
		result = Converter::strToInt( s );
	}
	return result;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

vector<string> FileSystem::getFileList( string path,
			   const vector<string> &extensions, bool addPathToResult,
			   int fromFrame, int toFrame  )
//упор€доченный по номерам
{
	path = pathNormalize( path );

	TStringList *list = new TStringList();
	list->Sorted = true;

	if (path != "") {
		string mask = path + "*.*"; //filter;

		TSearchRec sr;
		int iAttributes = 0;
		iAttributes |= faAnyFile;

		if (FindFirst(mask.c_str(), iAttributes, sr) == 0)	{
			do {
			  if ((sr.Attr & iAttributes) == sr.Attr) {
				//res.push_back( sr.Name.c_str() );

				//проверка, что подходит расширение
				string ext = LowerCase( ExtractFileExt( sr.Name ) ).c_str();

				bool ok = false;
				for (int i=0; i<extensions.size() && !ok; i++) {
					if ( ext == "." + extensions[i] ) {
						ok = true;
					}
				}

				string fileName = sr.Name.c_str();
				int number = fileNumber( fileName );
				ok = (ok && number >= 0);
				if (ok && fromFrame >= 0) {
					ok = ( number >= fromFrame );
				}
				if (ok && toFrame >= 0) {
					ok = ( number <= toFrame );
				}

				if ( ok ) {
					list->Add( fileName.c_str() );
				}

			  }
			} while (FindNext(sr) == 0);
			FindClose(sr);
		}
	}

	//упор€дочение по номерам
	int n = list->Count;
	vector<int> num( n );
	for (int i=0; i<n; i++) {
		num[ i ] = fileNumber( list->Strings[i].c_str() ) ;
	}

	int number = -1;
	vector<string> res( list->Count );
	for (int i = 0; i < n; i++) {
		//ищем с бќльшим номером, но минимальный
		int bestJ = -1;
		int bestN = -1;
		for (int j = 0; j < n; j++ ) {
			if ( num[j] > number && ( bestJ == -1 || bestN > num[j] ) ) {
				bestN = num[j];
				bestJ = j;
			}
		}
		if ( bestJ != -1 ) {
			res[ i ] = list->Strings[ bestJ ].c_str();
			number = bestN;
		}
		if (addPathToResult) {
			res[ i ] = path + res[ i ];
		}
	}
	delete list;

	return res;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

string FileSystem::pathNormalize( string path )		 //добавить '\\' в конце
{
	string res = path;
	if (res!="") {
		if (res[ res.length()-1 ] != '\\') {
			res += '\\';
		}
	}
	return res;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
string FileSystem::extractPath( const string &fileName )    //выдел€ет путь до файла
{
	String sPath = ExtractFilePath( fileName.c_str() );
	return sPath.c_str();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

