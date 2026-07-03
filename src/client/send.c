#include "oeuf_socket.h"

ssize_t oeso_client_send(oeso_clt_ctx_t *ctx, void *buf, size_t len) {
	if (ctx->fd == INVALID_SOCKET)
		return -1;
	ssize_t ret = send(ctx->fd, (void *)buf, len, 0);
	if (ret >= 0)
		return ret;
	int is_fine = 0;
	#if defined(__linux__)
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			is_fine = 1;
	#else 
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			is_fine = 1;
	#endif
	if (is_fine)
		return 0;
	ctx->on_disconnect(ctx, ret != 0);
	SOCK_CLOSE(ctx->fd);
	ctx->fd = INVALID_SOCKET;
	ctx->is_connected = 0;
	return -1;
}
