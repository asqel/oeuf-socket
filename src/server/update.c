#include "oeuf_socket.h"

void oeso_server_update(oeso_server_ctx_t *ctx) {
	_oeso_check_accept(ctx);	
	_oeso_check_clients(ctx);
}

static int add_space(oeso_server_ctx_t *ctx) {
	if (ctx->clients_len < ctx->clients_capacity)
		return 0;
	oeso_server_client_t *new_ptr = realloc(ctx->clients, sizeof(oeso_server_client_t) * (ctx->clients_capacity + 16));
	if (!new_ptr)
		return 1;
	ctx->clients = new_ptr;
	ctx->clients_capacity += 16;
	return 0;
}

#if defined(_WIN32) || defined(__linux__)
void _oeso_check_accept(oeso_server_ctx_t *ctx) {
	struct sockaddr addr = {0};
	socklen_t addr_len = 0;

	oeso_socket_t fd = accept(ctx->fd, &addr, &addr_len);
	if (fd == INVALID_SOCKET)
		return ;

	if (_oeso_set_nonblock(fd))
		goto err;
	if (add_space(ctx))
		goto err;
	ctx->clients[ctx->clients_len].addr = addr;
	ctx->clients[ctx->clients_len].addr_len = addr_len;
	ctx->clients[ctx->clients_len].fd = fd;
	ctx->clients[ctx->clients_len].data = NULL;
	if (ctx->on_connect(&ctx->clients[ctx->clients_len], ctx))
		goto err;
	ctx->clients_len++;

	return ;
	
	err:
		if (fd != INVALID_SOCKET)
			SOCK_CLOSE(fd);
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif

#if defined(_WIN32) || defined(__linux__)
void _oeso_update_client(oeso_server_client_t *clt, oeso_server_ctx_t *ctx) {
	int is_fine = 0;
	while (1) {
		uint8_t buffer[4096 * 2];
		int ret = recv(clt->fd, (void *)buffer, sizeof(buffer), 0);
		if (ret > 0)
			ctx->on_recv(clt, ctx, buffer, (size_t)ret);
		else if (ret == 0) {
			is_fine = 1;
			goto err;
		}
		else {
			#if defined(__linux__)
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					is_fine = 1;
			#else 
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
					is_fine = 1;
			#endif
			if (!is_fine)
				goto err;
			break;
		}
	}
	return ;
	err:
		ctx->on_disconnect(clt, ctx, is_fine);
		SOCK_CLOSE(clt->fd);
		clt->fd = INVALID_SOCKET;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif

void _oeso_check_clients(oeso_server_ctx_t *ctx) {
	for (size_t i = 0; i < ctx->clients_len; i++) {
		oeso_server_client_t *clt = &ctx->clients[i];
		if (clt->fd != INVALID_SOCKET)
			_oeso_update_client(clt, ctx);
	}
	_oeso_clean_list(ctx);
}

void _oeso_clean_list(oeso_server_ctx_t *ctx) {
	size_t count = 0;
	for (size_t i = 0; i < ctx->clients_len; i++) {
		

		if (ctx->clients[i].fd != INVALID_SOCKET) {
			oeso_server_client_t tmp = ctx->clients[i];
			ctx->clients[i] = ctx->clients[count];
			ctx->clients[count] = tmp;
			count++;
		}
	}
	size_t idx = ctx->clients_len - 1;
	while (ctx->clients_len > 0) {
		if (ctx->clients[idx].fd != INVALID_SOCKET)
			break;
		ctx->clients_len--;
		idx--;
	}
}
