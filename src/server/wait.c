#include "oeuf_socket.h"

#if defined(_WIN32) || defined(__linux__)
void oeso_server_wait(int at_least_ms, oeso_server_ctx_t *ctx) {
	int64_t end = oeso_I_time_ms() + at_least_ms;
	while (at_least_ms < 0 || oeso_I_time_ms() < end) {
	}
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
