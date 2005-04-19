#include <iconv.h>

int main() {
	iconv_t ic;
	const char *in;
	size_t in_len;
	char *out;
	size_t out_len;
	iconv(ic, &in, &in_len, &out, &out_len);
	return 0;
}
