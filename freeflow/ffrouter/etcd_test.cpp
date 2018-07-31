// MIT License

// Copyright (c) 2018 Gang Liao <gangliao@cs.umd.edu>

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <b64.h>
#include <curl/curl.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <json/json.h>

size_t process_data_v2(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    Json::Value root;
    Json::Value node;
    Json::Reader reader;
    Json::FastWriter writer;
    std::string json = (char *)buffer;

    EXPECT_TRUE(reader.parse(json, root));

    std::string nodeString = writer.write(root["node"]);

    EXPECT_TRUE(reader.parse(nodeString, node));

    *(std::string *)user_p = writer.write(node["value"]);

    return size * nmemb;
}

TEST(ETCDv2, GetValue)
{
    static const char *pCertFile   = "/etc/etcd/ssl/etcd.pem";
    static const char *pCACertFile = "/etc/kubernetes/ssl/ca.pem";
    static const char *pKeyName    = "/etc/etcd/ssl/etcd-key.pem";
    static const char *pKeyType    = "PEM";

    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_SSL);

    EXPECT_EQ(CURLE_OK, return_code) << "init libcurl failed.";

    CURL *easy_handle = curl_easy_init();
    CHECK_NOTNULL(easy_handle);

    std::string buff_p;

    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://10.142.104.73/v2/keys/Microsoft");
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379);

    /* since PEM is default, we needn't set it for PEM */
    curl_easy_setopt(easy_handle, CURLOPT_SSLCERTTYPE, "PEM");

    /* set the cert for client authentication */
    curl_easy_setopt(easy_handle, CURLOPT_SSLCERT, pCertFile);

    /* set the private key (file or ID in engine) */
    curl_easy_setopt(easy_handle, CURLOPT_SSLKEYTYPE, pKeyType);
    curl_easy_setopt(easy_handle, CURLOPT_SSLKEY, pKeyName);

    /* set the file with the certs vaildating the server */
    curl_easy_setopt(easy_handle, CURLOPT_CAINFO, pCACertFile);

    /* disconnect if we can't validate server's cert */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_data_v2);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &buff_p);

    CURLcode res = curl_easy_perform(easy_handle);

    EXPECT_EQ(res, CURLE_OK) << curl_easy_strerror(res);

    std::cout << buff_p << std::endl;

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}

struct MemoryStruct
{
    char *memory;
    size_t size;
};

size_t process_range_v3(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    size_t realsize          = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)user_p;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

TEST(ETCDv3, GetKVUnderDirectory)
{
    static const char *pCACertFile = "/etc/kubernetes/ssl/ca.pem";

    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_SSL);

    EXPECT_EQ(CURLE_OK, return_code) << "init libcurl failed.";

    CURL *easy_handle = curl_easy_init();
    CHECK_NOTNULL(easy_handle);

    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://10.142.104.73/v3alpha/kv/range");
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379);

    struct curl_slist *headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);

    std::string key       = "Microsoft/FreeFlow";
    char *encoded_key     = b64_encode((const unsigned char *)key.c_str(), key.length());
    char *encoded_end_key = b64_encode((const unsigned char *)key.c_str(), key.length());

    LOG(INFO) << "base64 encoding [Microsoft] to [" << encoded_key << "]";

    // If range_end is key plus one (e.g., "aa"+1 == "ab", "a\xff"+1 == "b"), then the
    // range represents all keys prefixed with key.
    size_t n               = strlen(encoded_end_key);
    encoded_end_key[n - 1] = encoded_key[n - 1] + 1;

    char post_fields[1024];
    sprintf(post_fields, "{\"key\": \"%s\", \"range_end\": \"%s\"}", encoded_key, encoded_end_key);
    curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, post_fields);

    /* set the file with the certs vaildating the server */
    curl_easy_setopt(easy_handle, CURLOPT_CAINFO, pCACertFile);
    /* disconnect if we can't validate server's cert */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_range_v3);

    struct MemoryStruct chunk;

    chunk.memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
    chunk.size   = 0;                 /* no data at this point */
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    CURLcode res = curl_easy_perform(easy_handle);

    EXPECT_EQ(res, CURLE_OK) << curl_easy_strerror(res);

    Json::Value root;
    Json::Value kv;
    Json::Reader reader;
    std::string json = chunk.memory;

    EXPECT_TRUE(reader.parse(json, root));

    kv = root["kvs"];

    if (!kv.empty())
    {
        for (int i = 0; i < kv.size(); ++i)
        {
            std::string key = kv[i]["key"].asString();
            std::string val = kv[i]["value"].asString();

            key = (char *)b64_decode(key.c_str(), key.length());
            val = (char *)b64_decode(val.c_str(), val.length());

            LOG(INFO) << key << "\t" << val;
        }
    }

    free(chunk.memory);
    free(encoded_key);
    free(encoded_end_key);

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}

size_t process_watch_v3(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    Json::Value root;
    Json::Value kv;
    Json::Reader reader;
    std::string json = (char *)buffer;

    LOG(INFO) << "parsing json: " << json << std::endl;

    EXPECT_TRUE(reader.parse(json, root));

    kv = root["result"]["events"];

    if (!kv.empty())
    {
        if (kv[0]["type"].asString() != "DELETE")
        {
            std::string key = kv[0]["kv"]["key"].asString();
            std::string val = kv[0]["kv"]["value"].asString();

            key = (char *)b64_decode(key.c_str(), key.length());
            val = (char *)b64_decode(val.c_str(), val.length());

            LOG(INFO) << key << " : " << val;
        }
    }

    return size * nmemb;
}

TEST(ETCDv3, WatchValueChange)
{
    static const char *pCACertFile = "/etc/kubernetes/ssl/ca.pem";

    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_SSL);

    EXPECT_EQ(CURLE_OK, return_code) << "init libcurl failed.";

    CURL *easy_handle = curl_easy_init();
    CHECK_NOTNULL(easy_handle);

    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://10.142.104.73/v3alpha/watch");
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379);

    struct curl_slist *headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);

    std::string key       = "Microsoft/FreeFlow";
    char *encoded_key     = b64_encode((const unsigned char *)key.c_str(), key.length());
    char *encoded_end_key = b64_encode((const unsigned char *)key.c_str(), key.length());

    LOG(INFO) << "base64 encoding [Microsoft] to [" << encoded_key << "]";

    // If range_end is key plus one (e.g., "aa"+1 == "ab", "a\xff"+1 == "b"), then the
    // range represents all keys prefixed with key.
    size_t n               = strlen(encoded_end_key);
    encoded_end_key[n - 1] = encoded_key[n - 1] + 1;

    char post_fields[1024];
    sprintf(post_fields, "{\"create_request\": {\"key\": \"%s\", \"range_end\": \"%s\"}}", encoded_key, encoded_end_key);
    curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, post_fields);

    /* set the file with the certs vaildating the server */
    curl_easy_setopt(easy_handle, CURLOPT_CAINFO, pCACertFile);
    /* disconnect if we can't validate server's cert */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_watch_v3);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, NULL);

    CURLcode res = curl_easy_perform(easy_handle);

    EXPECT_EQ(res, CURLE_OK) << curl_easy_strerror(res);

    free(encoded_key);
    free(encoded_end_key);

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}
