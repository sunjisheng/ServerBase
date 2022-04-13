#pragma once
namespace Minicat
{
	const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	/* Base64 ±àÂë */
	char* Base64_Encode(const char* data, int data_len);
	/* Base64 ½âÂë */
	char *Base64_Decode(const char* data, int data_len);
}
