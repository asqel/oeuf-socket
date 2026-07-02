#include "oeuf_socket.h"

static oeso_socket_t get_fd(const char *ip, int port) {
	oeso_socket_t res = INVALID_SOCKET;
	struct addrinfo hints = {0};
	struct addrinfo *info = {0};

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(ip, NULL, &hints, &info))
		return INVALID_SOCKET;
	
	for (struct addrinfo *ptr = info; ptr; ptr = ptr->ai_next) {
		res = socket(AF_INET, SOCK_STREAM, 0);
		if (res == INVALID_SOCKET)
			continue;
		((struct sockaddr_in *)ptr->ai_addr)->sin_port = port;
		if (!connect(res, ptr->ai_addr, ptr->ai_addrlen)) {
			freeaddrinfo(info);
			return res;
		}
		SOCK_CLOSE(res);
	}
	freeaddrinfo(info);
	return INVALID_SOCKET;
}

int oeso_client_init(oeso_clt_ctx_t *ctx, const char *ip, int port) {
	oeso_socket_t fd = get_fd(ip, port);
	if (_oeso_set_nonblock(fd)) {
		SOCK_CLOSE(fd);
		return -1;
	}
	ctx->fd = fd;
	return 0;
}
