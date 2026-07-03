#include "oeuf_socket.h"

void oeso_client_update(oeso_clt_ctx_t *ctx) {
	while (ctx->fd != INVALID_SOCKET) {
		uint8_t buffer[4096 * 2];
		int ret = recv(ctx->fd, (void *)buffer, sizeof(buffer), 0);
		if (ret > 0)
			ctx->on_recv(ctx, buffer, ret);
		else if (ret == 0) {
			ctx->on_disconnect(ctx, 1);
			SOCK_CLOSE(ctx->fd);
			ctx->fd = INVALID_SOCKET;
			ctx->is_connected = 0;
		}
		else if (ret < 0) {
			int is_fine = 0;
			#if defined(_WIN32)
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					is_fine = 1;
			#elif defined(__linux__)
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					is_fine = 1;
			#else
				#error "Unsupported OS " __LINE__ " file " __FILE__
			#endif
			if (is_fine)
				continue;
			ctx->on_disconnect(ctx, 0);
			SOCK_CLOSE(ctx->fd);
			ctx->fd = INVALID_SOCKET;
			ctx->is_connected = 0;
		}
	}
}
