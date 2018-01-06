#ifndef pbFilesH
#define pbFilesH


#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


class pbFiles
{
public:
    static bool fileExists(const string &fileName);
    //static int fileSize(  const string &fileName );
    static vector<string> readStrings(const string &fileName);

};

#endif