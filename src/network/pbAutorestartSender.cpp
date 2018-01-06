#include "pbAutorestartSender.h"


//Отправляет сообщения:
//run
//update
//update
//update
//...
//exit

//--------------------------------------------------------------
void pbAutorestartSender::setup(bool enabled, int port, float sendLoopSec, bool verbose)
{
    _enabled = enabled;
    _port = port;
    _sendLoopSec = sendLoopSec;
    _verbose = verbose;
    cout << "AutorestartSender: enabled=" << _enabled
         << " port=" << _port
         << " sendLoopSec=" << _sendLoopSec
         << " verbose=" << _verbose
         << endl;
    _sender.setup("127.0.0.1", port, 0);

    _time = 0;
    sendMessage("run");

}

//--------------------------------------------------------------
void pbAutorestartSender::exit()		//пользователь нажал exit, надо это послать, срочной отправкой
{
    if (_enabled) {
        //		if ( _verbose ) cout << "AutorestartSender exit" << endl;
        sendMessage("exit");
        ofSleepMillis(1000);
        _sender.close();
    }
}

//--------------------------------------------------------------
void pbAutorestartSender::sendMessage(const string &message)
{
    if (_verbose) {
        cout << "AutorestartSender message: " << message << endl;
    }
    _time = ofGetElapsedTimef();
    _sender.addString("/autorestart", message);
    _sender.flush();
}

//--------------------------------------------------------------
void pbAutorestartSender::update()
{
    if (_enabled) {
        float time = ofGetElapsedTimef();
        if (time > _time + _sendLoopSec) {
            sendMessage("update");
        }
    }
}

//--------------------------------------------------------------