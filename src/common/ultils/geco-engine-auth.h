#ifndef _INCLUDE_GECO_ENGINE_AUTH
#define _INCLUDE_GECO_ENGINE_AUTH

/*=========== BASE64 ===============*/
#include <string>
std::string base64_encode(const char* data, size_t len);
std::string base64_encode(const std::string & data)
{
    return base64_encode(data.data(), data.size());
}

int base64_decode(const std::string& data, char* results, size_t bufSize);
bool base64_decode(const std::string & inData, std::string & outData);
#endif