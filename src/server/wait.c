#include "oeuf_socket.h"

#if defined(_WIN32) || defined(__linux__)
static int wait_events(int64_t timeout, oeso_srv_ctx_t *ctx) {
	#if defined(_WIN32)
		WSAPOLLFD *fds = malloc(sizeof(WSAPOLLFD) * (1 + ctx->clients_len));
	#else
		struct pollfd *fds = malloc(sizeof(struct pollfd) * (1 + ctx->clients_len));
	#endif
	if (!fds)
		return 0;

	for (size_t i = 0; i < ctx->clients_len; i++) {
		fds[i+ 1].fd = ctx->clients[i].fd;
		fds[i+ 1].revents = 0;
		fds[i+ 1].events = POLLIN;
		if (ctx->need_send)
			fds[i + 1].events |= POLLOUT;
	}
	fds[0].fd = ctx->fd;
	fds[0].revents = 0;
	fds[0].events = POLLIN;

	#if defined(_WIN32)
		int ret = WSAPoll(fds, 1 + ctx->clients_len, timeout);
	#else
		int ret = poll(fds, 1 + ctx->clients_len, timeout);
	#endif
	if (ret <= 0) {
		free(fds);
		return 0;
	}
	int has_disco = 0;
	for (size_t i = 0; (i < ctx->clients_len + 1) && (ret > 0); i++) {
		if (i == 0 && fds[i].revents & POLLIN) {
			_oeso_check_accept(ctx);
			ret--;
		}
		else if (fds[i].revents & POLLIN) {
			_oeso_update_client(&ctx->clients[i - 1], ctx);
			if (ctx->clients[i - 1].fd == INVALID_SOCKET)
				has_disco = 1;
			ret--;
		}
		else if (fds[i].revents & POLLOUT) {
			if (ctx->need_send)
				ctx->on_send_ready(&ctx->clients[i - 1], ctx);
			ret--;
		}
	}
	free(fds);
	return has_disco;
}

void oeso_server_wait(oeso_srv_ctx_t *ctx, int at_least_ms) {
	int64_t end = _oeso_time_ms() + at_least_ms;
	while (at_least_ms < 0 || _oeso_time_ms() < end) {
		if (at_least_ms < 0)
			wait_events(-1, ctx);
		else {
			int64_t timeout = end - _oeso_time_ms();
			if (timeout < 0)
				break;
			if (wait_events(timeout, ctx))
				_oeso_clean_list(ctx);
		}
	}
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
