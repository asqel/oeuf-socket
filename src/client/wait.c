#include "oeuf_socket.h"

static void check_connect(oeso_clt_ctx_t *ctx) {
	int error = 0;
	#if defined(_WIN32)
		int len = sizeof(int);
		getsockopt(ctx->fd, SOL_SOCKET, SO_ERROR, (void *)&error, &len);
	#else
		socklen_t len = sizeof(int);
		getsockopt(ctx->fd, SOL_SOCKET, SO_ERROR, &error, &len);
	#endif
	ctx->is_connected = !error;
	ctx->on_connect(ctx, ctx->is_connected);
	if (error) {
		SOCK_CLOSE(ctx->fd);
		ctx->fd = INVALID_SOCKET;
	}
}

static void wait_events(int64_t timeout, oeso_clt_ctx_t *ctx) {
	#if defined(_WIN32)
		WSAPOLLFD fds = {0};
	#else
		struct pollfd fds = {0};
	#endif

	fds.fd = ctx->fd;
	fds.revents = 0;
	fds.events = POLLIN;
	if (ctx->need_send || !ctx->is_connected)
		fds.events |= POLLOUT;
	if (!ctx->is_connected)
		fds.events |= POLLERR | POLLHUP;
	
	#if defined(_WIN32)
		int ret = WSAPoll(&fds, 1, timeout);
	#else
		int ret = poll(&fds, 1, timeout);
	#endif
	if (ret <= 0)
		return ;
	if ((fds.revents & POLLOUT) && ctx->need_send)
		ctx->on_send_ready(ctx);
	if (((fds.revents & POLLOUT) || (fds.revents & POLLERR) || (fds.revents & POLLHUP)) && !ctx->is_connected)
		check_connect(ctx);
	if (fds.revents & POLLIN) 
		oeso_client_update(ctx);
}

#if defined(_WIN32)
void usleep(int64_t usec) {
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec);

    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

#endif

void oeso_client_wait(oeso_clt_ctx_t *ctx, int at_least_ms) {
	int64_t end = _oeso_time_ms() + at_least_ms;
	while (at_least_ms < 0 || _oeso_time_ms() < end) {
		if (at_least_ms < 0)
			wait_events(-1, ctx);
		else if (ctx->fd != INVALID_SOCKET) {
			int64_t timeout = end - _oeso_time_ms();
			if (timeout < 0)
				break;
			wait_events(timeout, ctx);
		}
		else {
			int64_t timeout = end - _oeso_time_ms();
			usleep(timeout * 1000);
		}
	}
}
