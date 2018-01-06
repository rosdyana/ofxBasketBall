#include "pbWebServer.h"
#include "ofxIniSettings.h"
#include "pbSystem.h"

#include "mongoose.h"


struct mg_context *_server = NULL;
pbWebServerCallbackCommand _webserver_callback = 0;

static const char *webserver_sendcommand_url = "/command";			//url ��� �������� �������
static const char *webserver_sendcommand_command = "cmd";			//�������� ��� �������� �������
static const char *webserver_sendcommand_indexPage = "indexPage";	//������� �� �� index� true/false
static const char *webserver_sendcommand_page = "page";				//������� �� ��������.


//������:
//host/command?cmd=����������


static const char *webserver_reply_start =						//������ ������ �������
"HTTP/1.1 200 OK\r\n"
"Cache: no-cache\r\n"
"Content-Type: text/html\r\n"
"\r\n";

//---------------------------------
//����� �������� ���������
static void get_qsvar(const struct mg_request_info *request_info,
					  const char *name, char *dst, size_t dst_len) {
						  const char *qs = request_info->query_string;
						  mg_get_var(qs, strlen(qs == NULL ? "" : qs), name, dst, dst_len);
}

//---------------------------------
//�������� �� index.html
static void redirect_to_index(struct mg_connection *conn,
							const struct mg_request_info *request_info) 
{
	mg_printf(conn, "HTTP/1.1 302 Found\r\n"
		//"Set-Cookie: original_url=%s\r\n"
		"Location: %s\r\n\r\n",
		//request_info->uri, 
		"/");
}

//---------------------------------
//�������� �� ��������
static void redirect_to_page(struct mg_connection *conn,
							const struct mg_request_info *request_info, string page) 
{
	mg_printf(conn, "HTTP/1.1 302 Found\r\n"
		//"Set-Cookie: original_url=%s\r\n"
		"Location: %s\r\n\r\n",
		//request_info->uri, 
		page.c_str());
}

//---------------------------------
//��������� �������
static void webserver_sendcommand(struct mg_connection *conn,
								  const struct mg_request_info *request_info)  
{
	//��������� �������
	string command = "";
	char cb[2048];
	get_qsvar(request_info, webserver_sendcommand_command, cb, sizeof(cb));
	if (cb[0] != '\0') {
		//mg_printf(conn, "%s", cb);
		command = cb;
	}

	//������, ���� �� ������� �� �����-�� ��������	
	string gotoPage = "";
	char stayb[2048];
	get_qsvar(request_info, webserver_sendcommand_indexPage, stayb, sizeof(stayb));
	if ( string( stayb ) == "true" ) {
		gotoPage = "/";	
	}
	//������, ����� ���� ������� �� �����-�� ��������
	get_qsvar(request_info, webserver_sendcommand_page, stayb, sizeof(stayb));
	if ( string( stayb ) != "" ) {
		gotoPage = string( stayb );	
	}


	//��������� ������
	string response = "";
	//if ( command != "" ) {
		if ( _webserver_callback ) {
			_webserver_callback( command, response );
		}
		else {
			response = "Internal error: no callback";
		}
	//}
	//else {
	//	stayAtIndex = true;	
	//}

	
	

	//�����
	if ( gotoPage == "" ) {
		//������ ������
		mg_printf(conn, "%s", webserver_reply_start);
		// get_qsvar(request_info, "last_id", last_id, sizeof(last_id));
		//��� �����
		//mg_printf(conn, "%s", "Received command=");
		mg_printf(conn, "%s", response.c_str());
	}
	else {
		redirect_to_page( conn, request_info, gotoPage );
	}
}


//---------------------------------
//����� ���������� �������
static void *webserver_event_handler(enum mg_event ev,
struct mg_connection *conn,
	const struct mg_request_info *request_info) 
{
	void *processed = "yes";
	if (ev == MG_NEW_REQUEST) {
		if (strcmp(request_info->uri, webserver_sendcommand_url) == 0) {
			webserver_sendcommand(conn, request_info);
		}
		//else if () {		//������ �������
		//}
		else {
			// No suitable handler found, mark as not processed. Mongoose will
			// try to serve the request.
			processed = NULL;
		}
	} else {
		processed = NULL;
	}
	return processed;
}



//--------------------------------------------------------------
//����� ����������
void pbWebServer::setup( bool enable,				//�������� �� ������
						const string &fileName,					//���� ������������
						pbWebServerCallbackCommand callback		//callback
						)
{
	if ( enable ) {

		//��������� �������, ���� �� ��� �������
		close();

		//���������� ����������
		ofxIniSettings ini;
		ini.load( fileName );

		string path = "Webserver.";
		bool enabled			= ini.get( path + "enabled", 0 );
		string listeningPorts	= ini.get( path + "listeningPorts", string() );
		int threads				= ini.get( path + "threads", 5 );
		string logDir			= ini.get( path + "logDir", string() );
		string documentRoot		= ini.get( path + "documentRoot", string() );

		if ( enabled ) {
			//�������� ������� �����
			vector<string> opt;
			opt.push_back( "document_root" ); 
			opt.push_back( documentRoot );
			opt.push_back( "listening_ports" );
			opt.push_back( listeningPorts );
			opt.push_back( "num_threads" );
			opt.push_back( ofToString( threads ) );
			opt.push_back( "access_log_file" );
			opt.push_back( logDir + "/web_access.log" );
			opt.push_back(  "error_log_file" );
			opt.push_back( logDir + "/web_error.log" );
			//"enable_directory_listing", "yes",
			opt.push_back( "" );

			const char *options[100];
			for (int i=0; i<opt.size(); i++) {
				if ( opt[i] != "" ) options[i] = opt[i].c_str();
				else options[i] = NULL;
			}

			//����� �� �����
			cout << endl;
			cout << "------------------------------------" << endl;
			cout << "Starting Webserver" << endl;
			for (int i=0; i<opt.size(); i++ ) {
				if ( i % 2 == 0) cout << "    " << opt[i];
				else cout << ": " << opt[i] << endl;				
			}
			cout << endl;

			// Setup and start Mongoose
			_server = mg_start(&webserver_event_handler, options);


			//���� ������ �� ���������� - ����� �� ���������
			if ( _server == NULL ) {
				pbSystem::criticalError( "Can not start WebServer" );
			}
			cout << string("Web server started on ports ") + mg_get_option(_server, "listening_ports") << endl;
			cout << "------------------------------------" << endl;
			cout << endl;

			//��������� ������� ������������
			_webserver_callback = callback; 

		}
	}
}

//--------------------------------------------------------------
//��������� ����������, ������� ������
void pbWebServer::close()
{
	if ( _server ) {
		cout << "Stopping Webserver... ";
		mg_stop(_server);
		_server = NULL;
		cout << "   Stopped" << endl;
	}
}



//--------------------------------------------------------------
