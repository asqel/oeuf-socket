#include "oeuf_socket.h"

static int init_fd(oeso_srv_ctx_t *ctx, int port);

int oeso_server_init(oeso_srv_ctx_t *ctx, int port) {
	if (!ctx)
		return -1;
	if (init_fd(ctx, port))
		return -1;
	ctx->clients_len = 0;
	ctx->clients = NULL;
	ctx->clients_capacity = 0;
	return 0;
}

#if defined(_WIN32) || defined(__linux__)
static int init_fd(oeso_srv_ctx_t *ctx, int port) {
	oeso_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == INVALID_SOCKET)
		goto err;

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (bind(fd, (void *)&addr, sizeof(addr)) < 0)
		goto err;

	if (listen(fd, 256) < 0)
		goto err;

	if (_oeso_set_nonblock(fd))
		goto err;

	ctx->fd = fd;
	return 0;

	err:
		if (fd != INVALID_SOCKET)
			SOCK_CLOSE(fd);
		return -1;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__
	
#endif
