#pragma once

#include <windows.h>
#include <process.h>
#include <stdio.h>

//Using Events
//http://msdn.microsoft.com/en-us/library/ms686360%28v=vs.85%29.aspx
//http://msdn.microsoft.com/en-us/library/ms686915%28v=vs.85%29.aspx

//--------------------------------------------------------------
typedef HANDLE pbEventType;

class pbEvent
{
public:
	pbEvent(void) { _ev = NULL; _verbose = false; }
	~pbEvent(void) {}

	bool setup( bool manualReset, bool verbose ) 
	{
		_verbose = verbose;

		_ev = CreateEventW( 
        NULL,               // default security attributes
        manualReset,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        L"pbEvent"  // object name
        ); 

		bool result = ( _ev != NULL );
		if ( !result ) { 
			printf("CreateEvent failed (%d)\n", GetLastError());
		}
		return result;
	}

	bool close()
	{
	    // Close all event handles (currently, only one global handle).
		CloseHandle( _ev );
		_ev = NULL;
		return true;
	}

	bool set()
	{
		if ( _verbose ) {
			printf("SetEvent \n");
		}
		bool result = SetEvent( _ev ); 
		if ( !result ) {
			printf("SetEvent failed (%d)\n", GetLastError());
		}
		return result;
	}

	bool reset(){
		if ( _verbose ) {
			printf("ResetEvent \n");
		}

		bool result = ResetEvent( _ev );
		if ( !result ) {
			printf("ResetEvent failed (%d)\n", GetLastError());
		}
		return result;
	}

	bool waitInfinite()
	{
		if ( _verbose ) {
			printf("Thread %d waiting for event...\n", GetCurrentThreadId());
		}

		DWORD  dwWaitResult = WaitForSingleObject( _ev, INFINITE );   
		bool result = ( dwWaitResult == WAIT_OBJECT_0 );
		if ( !result ) {
			printf("Wait error (%d)\n", GetLastError()); 
		}
		else {
			if ( _verbose ) {
				printf("Thread %d finished waiting\n", GetCurrentThreadId());
			}
		}
		return result;
    }

private:
	pbEventType _ev;
	bool _verbose;
};






