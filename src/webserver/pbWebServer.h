#pragma once

/*
Веб-сервер для удаленного управления программами,
на основе mongoose http://code.google.com/p/mongoose/ 
Сейчас только один сервер может существовать в приложении, поэтому все члены - статические
Использование:

#include "pbWebServer.h"

//отработка команд веб-сервера
void webserver_callback( const string &in, string &out )
{
	//help - список команд
	//status - текущее состояние
	//slideNext, slidePrev,...
	out="command : " + in;
}

setup():
	pbWebServer::setup( true, "data/settings.ini", webserver_callback );

exit():
	pbWebServer::close();

в settings.ini:
%------------ Веб-сервер --------
[Webserver]
//Включать ли веб-сервер (1/0)
enabled=1

//Список прослушиваемых портов через запятую
listeningPorts=8081

//Число потоков, запускаемых для обработки запросов
//значение по умолчанию 5
threads=2

//Папка для index.html
documentRoot=data/webserver/html

//Папка для записи логов
logDir=data/webserver/logs

//----------
Формат команд:
host/  - показывает index.html из папки webserver/html
host/command?cmd=COMMANDNAME - вызывает callback с COMMANDNAME
если еще параметр &indexPage=true - то оставаться на странице index, и не выдавать результат работы команды
если еще параметр &page=index1.html, например, то перейти на index1.html

*/



#include "ofMain.h"


typedef void (* pbWebServerCallbackCommand)( const string &in, string &out );

class pbWebServer
{
public:
	//Старт вебсервера
	static void setup( bool enable,				//включать ли сервер
			const string &fileName,					//файл конфигурации
			pbWebServerCallbackCommand callback		//callback
			);

	//Остановка вебсервера, очистка памяти
	static void close();
private:
	
};

