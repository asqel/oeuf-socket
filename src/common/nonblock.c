#include "oeuf_socket.h"

#if defined(_WIN32)
int _oeso_set_nonblock(oeso_socket_t fd) {
	u_long mode = 1;
	if (ioctlsocket(fd,  FIONBIO, &mode))
		return -1;
	return 0;
}

#elif defined(__linux__)
int _oeso_set_nonblock(oeso_socket_t fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return -1;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return -1;
	return 0;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE

#endif

