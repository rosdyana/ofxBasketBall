#include "testApp.h"

#include "RunInfo.h"


#include "highgui.h"

#include "pbBaseApp.h"
#include "App.h"

App app;

//--------------------------------------------------------------
void testApp::setup()
{
	app.setup0();
}

//--------------------------------------------------------------
void testApp::exit()
{
	app.exit0();
}

//--------------------------------------------------------------
void testApp::update()
{
	app.update0();	
}

//--------------------------------------------------------------
//Рисование
void testApp::draw()
{
	app.draw0();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{
	app.keyPressed0( key );
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
	app.mouseMoved0( x, y );
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
	app.mouseMoved0( x, y );
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	app.mousePressed0( x, y, button );
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	app.mouseReleased0( x, y, button );
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
