#pragma once

//�������� ��������� ��������� ������������, ������� ������������ ��� ��������� � ������ �������

#include "ofMain.h"
#include "pbSenderOSC.h"

class pbAutorestartSender
{
public:
    void setup(bool enabled, int port, float sendLoopSec, bool verbose);
    void exit();		//������������ ����� exit, ���� ��� �������, ������� ���������
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
