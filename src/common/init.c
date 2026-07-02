#include "oeuf_socket.h"

int oeso_init() {
	#if defined(_WIN32)
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
			return -1;
	#endif
	return 0;
}
