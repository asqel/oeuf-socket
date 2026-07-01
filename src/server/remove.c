#include "oeuf_socket.h"

#if defined(_WIN32) || defined(__linux__)
void oeso_server_remove(oeso_srv_ctx_t *ctx, oeso_srv_clt_t *clt) {
	if (clt->fd == INVALID_SOCKET)
		return ;
	ctx->on_disconnect(clt, ctx, 1);
	SOCK_CLOSE(clt->fd);
	clt->fd = INVALID_SOCKET;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
