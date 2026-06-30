#include "oeuf_socket.h"

#if defined(_WIN32) || defined(__linux__)
ssize_t oeso_server_send(oeso_server_ctx_t *ctx, oeso_server_client_t *clt, void *buf, size_t len) {
	if (clt->fd == INVALID_SOCKET)
		return -1;
	ssize_t ret = send(clt->fd, buf, len, 0);
	if (ret >= 0)
		return ret;
	int is_fine = 0;
	#if defined(__linux__)
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			is_fine = 1;
	#else 
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK)
			is_fine = 1;
	#endif
	if (is_fine)
		return 0;
	oeso_server_remove(ctx, clt);
	return -1;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
