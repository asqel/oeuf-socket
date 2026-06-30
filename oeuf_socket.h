#ifndef SOCKET_OEUF_H
#define SOCKET_OEUF_H

#include <stdint.h>
#include <stdlib.h>

#if defined(_WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>

	typedef SOCKET oeso_socket_t;	
	#define SOCK_CLOSE closesocket

#elif defined(__linux__)
	#include <sys/socket.h>
	#include <time.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <stddef.h>
	#include <errno.h>

	typedef int oeso_socket_t;
	#undef INVALID_SOCKET
	#define INVALID_SOCKET (-1)
	#define SOCK_CLOSE close

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif

typedef struct oeso_server_client_t oeso_server_client_t;
typedef struct oeso_server_ctx_t oeso_server_ctx_t;

struct oeso_server_client_t {
	oeso_socket_t fd;

	struct sockaddr addr;
	socklen_t addr_len;

	void *data;
};

struct oeso_server_ctx_t {
	oeso_socket_t fd;

	size_t max_clt_data;
	int (*on_connect)(oeso_server_client_t *, oeso_server_ctx_t *); // return 1 to refuse
	void (*on_disconnect)(oeso_server_client_t *, oeso_server_ctx_t *, int); // int == 0 if reset
	void (*on_recv)(oeso_server_client_t *, oeso_server_ctx_t *, void *, size_t);
	void (*on_send_ready)(oeso_server_client_t *, oeso_server_ctx_t *); // used by wait

	oeso_server_client_t *clients;
	size_t clients_len;
	size_t clients_capacity;

	void *data;
};

int oeso_server_init(oeso_server_ctx_t *ctx, int port);
void oeso_server_update(oeso_server_ctx_t *ctx);
void oeso_server_remove(oeso_server_ctx_t *ctx, oeso_server_client_t *clt);

// return < 0 if connection ended ( == 0 if full for now)
ssize_t oeso_server_send(oeso_server_ctx_t *ctx, oeso_server_client_t *clt, void *buf, size_t len);

// can call ctx->on_send_ready
void oeso_server_wait(int at_least_ms, oeso_server_ctx_t *ctx);

// internal stuff
int oeso_I_set_nonblock(oeso_socket_t fd);
int64_t oeso_I_time_ms();

#endif
