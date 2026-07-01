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
	#include <poll.h>
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

typedef struct oeso_srv_clt_t oeso_srv_clt_t;
typedef struct oeso_srv_ctx_t oeso_srv_ctx_t;

struct oeso_srv_clt_t {
	oeso_socket_t fd;

	struct sockaddr addr;
	socklen_t addr_len;

	void *data;
};

struct oeso_srv_ctx_t {
	oeso_socket_t fd;

	size_t max_clt_data;
	int (*on_connect)(oeso_srv_clt_t *, oeso_srv_ctx_t *); // return 1 to refuse
	void (*on_disconnect)(oeso_srv_clt_t *, oeso_srv_ctx_t *, int); // int == 0 if reset
	void (*on_recv)(oeso_srv_clt_t *, oeso_srv_ctx_t *, void *, size_t);
	void (*on_send_ready)(oeso_srv_clt_t *, oeso_srv_ctx_t *); // used by wait
	uint8_t need_send; // set to 1 to receiv on_send_ready when wait

	oeso_srv_clt_t *clients;
	size_t clients_len;
	size_t clients_capacity;

	void *data;
};

// server side
int oeso_server_init(oeso_srv_ctx_t *ctx, int port);
void oeso_server_update(oeso_srv_ctx_t *ctx);
void oeso_server_remove(oeso_srv_ctx_t *ctx, oeso_srv_clt_t *clt);
// return < 0 if connection ended ( == 0 if full for now)
ssize_t oeso_server_send(oeso_srv_ctx_t *ctx, oeso_srv_clt_t *clt, void *buf, size_t len);
// can call ctx->on_send_ready
void oeso_server_wait(oeso_srv_ctx_t *ctx, int at_least_ms);
void oeso_server_exit(oeso_servre_ctx_t *ctx);


// client side
int oeso_client_init(oeso_clt_ctx_t *ctx, int port, const char *ip);



// internal stuff
int _oeso_set_nonblock(oeso_socket_t fd);
int64_t _oeso_time_ms();
void _oeso_check_accept(oeso_srv_ctx_t *ctx);
void _oeso_check_clients(oeso_srv_ctx_t *ctx);
void _oeso_update_client(oeso_srv_clt_t *clt, oeso_srv_ctx_t *ctx);
void _oeso_clean_list(oeso_srv_ctx_t *ctx);

#endif
