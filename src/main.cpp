#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "RunInfo.h"

//========================================================================
int main(int argc, char *argv[])
{



    ofAppGlutWindow window;

    runInfo.setup(argc, argv, &window);

    ofRunApp(new testApp());


}
