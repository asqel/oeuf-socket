#include "oeuf_socket.h"

void oeso_server_exit(oeso_servre_ctx_t *ctx) {
	if (ctx->fd != INVALID_SOCKET) {
		SOCK_CLOSE(ctx->fd);
		ctx->fd = INVALID_SOCKET;
	}
	for (size_t i = 0; i < ctx->clients_len; i++) {
		oeso_srv_clt_t *clt = &ctx->clients[i];
		oeso_server_remove(ctx, clt);
	}
	free(ctx->clients);
	ctx->clients = NULL;
	ctx->clients_len = 0;
	ctx->clients_capacity = 0;
}
