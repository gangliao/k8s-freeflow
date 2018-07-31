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

    if (!reader.parse(json, root))
    {
        std::cout << "parse json error" << std::endl;
        return 0;
    }
    std::string nodeString = writer.write(root["node"]);
    if (!reader.parse(nodeString, node))
    {
        std::cout << "parse json error" << std::endl;
        return 0;
    }

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
    /* Check for errors */
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    std::cout << buff_p << std::endl;

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}

size_t process_data_v3(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    Json::Value root;
    Json::Value kvs;
    Json::Reader reader;
    Json::FastWriter writer;
    std::string json = (char *)buffer;

    std::cout << json << std::endl;

    if (!reader.parse(json, root))
    {
        std::cout << "parse json error" << std::endl;
        return 0;
    }
    std::string kvsString = writer.write(root["kvs"][0]);
    if (!reader.parse(kvsString, kvs))
    {
        std::cout << "parse json error" << std::endl;
        return 0;
    }

    *(std::string *)user_p = writer.write(kvs["value"]);

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

    std::string buff_p;

    curl_easy_setopt(easy_handle, CURLOPT_URL, "https://10.142.104.73/v3alpha/watch");
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379);

    struct curl_slist *headers = NULL;
    headers                    = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers);

    std::string key   = "Microsoft";
    char *encoded_key = b64_encode((const unsigned char *)key.c_str(), key.length());

    LOG(INFO) << "base64 encoding [Microsoft] to [" << encoded_key << "]";

    char post_fields[1024];
    sprintf(post_fields, "{\"create_request\": {\"key\": \"%s\"}}", encoded_key);
    curl_easy_setopt(easy_handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy_handle, CURLOPT_POSTFIELDS, post_fields);

    /* set the file with the certs vaildating the server */
    curl_easy_setopt(easy_handle, CURLOPT_CAINFO, pCACertFile);
    /* disconnect if we can't validate server's cert */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 1L);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_data_v3);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, &buff_p);

    CURLcode res = curl_easy_perform(easy_handle);
    /* Check for errors */
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    std::cout << buff_p << std::endl;

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}
