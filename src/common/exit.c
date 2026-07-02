#include "oeuf_socket.h"

void oeso_exit() {
	#if defined(_WIN32)
		WSACleanup();
	#endif
}
