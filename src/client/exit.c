#include "oeuf_socket.h"

void oeso_client_exit(oeso_clt_ctx_t *ctx) {
	if (ctx->fd != INVALID_SOCKET)
		SOCK_CLOSE(ctx->fd);
	ctx->fd = INVALID_SOCKET;
}
