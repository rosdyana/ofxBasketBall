#pragma once

//Отправка сообщений программе авторестарта, которая перезапустит эту программу в случае падения

#include "ofMain.h"
#include "pbSenderOSC.h"

class pbAutorestartSender
{
public:
    void setup(bool enabled, int port, float sendLoopSec, bool verbose);
    void exit();		//пользователь нажал exit, надо это послать, срочной отправкой
    void update();


private:
    bool _enabled;
    int _port;
    float _sendLoopSec;
    bool _verbose;

    pbSenderOSC _sender;
    float _time;

    void sendMessage(const string &message);

};
