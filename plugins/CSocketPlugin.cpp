#include <iostream>

extern "C" {

#include "semblis.h"
#include "ForeignMacros.h"

#include "../../c_socket/CSocket.h"

}

using namespace CSocket;

FOREIGN_FUNCTION(net_func_0_0) {
    FF_START;

    Socket* result = new Socket(UDP);

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(net_on_port_func_0_1) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to net must be an integer");
    
    Socket* result = new Socket(UDP, data_get_integer(curData));

    FF_RET_INT(result);
}

FOREIGN_FUNCTION(poll_func_0_3) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to poll must be a pointer (integer)");

    Socket *socket = (Socket *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to poll must be an integer");

    int timeSeconds = (int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to poll must be an integer");

    int timeMicroSeconds = (int)data_get_integer(curData);

    bool result = socket->poll(timeSeconds, timeMicroSeconds);
    
    if(result) 
    {
	FF_RET_TRUE;
    } else {
	FF_RET_FALSE;
    }
}

FOREIGN_FUNCTION(recv_func_0_2) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to poll must be a pointer (integer)");

    Socket *socket = (Socket *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to poll must be an integer");

    unsigned int maxBufferSize = (int)data_get_integer(curData);

    char *buffer = new char[maxBufferSize];
    // TODO: find a way to delete this buffer

    unsigned long address;
    unsigned short port;

    socket->recvfrom(buffer,
		     maxBufferSize,
		     address,
		     port);

    Data *buffer_d = eng_create_int((int)buffer);
    Data *maxBufferSize_d = eng_create_int(maxBufferSize);
    Data *address_d = eng_create_int(address);
    Data *port_d = eng_create_int(port);

    Data *next = eng_create_pair(port_d, DataNIL);
    Data *result = eng_create_pair(address_d, next);
    next = eng_create_pair(maxBufferSize_d, result);
    result = eng_create_pair(buffer_d, next);    

    FF_END;
    return result;
}

FOREIGN_FUNCTION(send_func_0_5) {
    FF_START;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 1 to poll must be a pointer (integer)");

    Socket *socket = (Socket *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 2 to poll must be a char *");

    char *buffer = (char *)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 3 to poll must be an integer");

    unsigned int bufferSize = (unsigned int)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 4 to poll must be an integer");

    unsigned long address = (unsigned long)data_get_integer(curData);

    FF_GET_NEXT_ARG;

    if(!FF_IS_INT)
	FF_RET_ERROR(L"arg 5 to poll must be an integer");

    unsigned short port = (unsigned short)data_get_integer(curData);
    std::cout << "received port: " << port << std::endl;

    socket->sendto(buffer,
		   bufferSize,
		   address,
		   port);
    
    FF_RET_TRUE;
}
