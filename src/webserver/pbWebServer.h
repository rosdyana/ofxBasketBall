#pragma once

/*
���-������ ��� ���������� ���������� �����������,
�� ������ mongoose http://code.google.com/p/mongoose/ 
������ ������ ���� ������ ����� ������������ � ����������, ������� ��� ����� - �����������
�������������:

#include "pbWebServer.h"

//��������� ������ ���-�������
void webserver_callback( const string &in, string &out )
{
	//help - ������ ������
	//status - ������� ���������
	//slideNext, slidePrev,...
	out="command : " + in;
}

setup():
	pbWebServer::setup( true, "data/settings.ini", webserver_callback );

exit():
	pbWebServer::close();

� settings.ini:
%------------ ���-������ --------
[Webserver]
//�������� �� ���-������ (1/0)
enabled=1

//������ �������������� ������ ����� �������
listeningPorts=8081

//����� �������, ����������� ��� ��������� ��������
//�������� �� ��������� 5
threads=2

//����� ��� index.html
documentRoot=data/webserver/html

//����� ��� ������ �����
logDir=data/webserver/logs

//----------
������ ������:
host/  - ���������� index.html �� ����� webserver/html
host/command?cmd=COMMANDNAME - �������� callback � COMMANDNAME
���� ��� �������� &indexPage=true - �� ���������� �� �������� index, � �� �������� ��������� ������ �������
���� ��� �������� &page=index1.html, ��������, �� ������� �� index1.html

*/



#include "ofMain.h"


typedef void (* pbWebServerCallbackCommand)( const string &in, string &out );

class pbWebServer
{
public:
	//����� ����������
	static void setup( bool enable,				//�������� �� ������
			const string &fileName,					//���� ������������
			pbWebServerCallbackCommand callback		//callback
			);

	//��������� ����������, ������� ������
	static void close();
private:
	
};

