#include "pbSystem.h"
#include "pbFiles.h"

//----------------------------------------------------------------------
void pbSystem::criticalError( const string &message, int exitCode )
{

	string s = "CRITICAL ERROR: " + message;

	vector<string> list;
	list.push_back( s );
	pbFiles::writeStrings( list, ofToDataPath("_criticalError.txt") );

	cout << s << endl;
	ofSleepMillis( 5000 );
	OF_EXIT_APP( exitCode );
}

//----------------------------------------------------------------------
