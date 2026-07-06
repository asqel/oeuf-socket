#include "oeuf_socket.h"

#if defined(_WIN32)
int64_t _oeso_time_ms() {
	int64_t wintime = 0;

	GetSystemTimeAsFileTime((FILETIME *)&wintime);
	wintime -= 116444736000000000LL;
	wintime /= 10000;

	static uint8_t is_init = 0;
	static int64_t time_start = 0;
	if (!is_init) {
		is_init = 1;
		time_start = wintime;
	}
	return wintime - time_start;
}

#elif defined(__linux__)
int64_t _oeso_time_ms() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	int64_t res = t.tv_sec * 1000 + t.tv_nsec / 1000000;

	static uint8_t is_init = 0;
	static int64_t time_start = 0;
	if (!is_init) {
		is_init = 1;
		time_start = res;
	}

	return res - time_start;
}

#else
	#error "Unsupported OS " __LINE__ " file " __FILE__

#endif
