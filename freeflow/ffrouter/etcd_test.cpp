#include <curl/curl.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <json/json.h>

using namespace std;
using namespace Json;

size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    Value root;
    Value node;
    Reader reader;
    FastWriter writer;
    string json = (char *)buffer;

    if (!reader.parse(json, root))
    {
        cout << "parse json error" << endl;
        return 0;
    }
    string nodeString = writer.write(root["node"]);
    if (!reader.parse(nodeString, node))
    {
        cout << "parse json error" << endl;
        return 0;
    }

    cout << node["value"] << endl;

    return size * nmemb;
}

TEST(ETCD, GetValue)
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

    char *buff_p = NULL;

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

    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buff_p);

    CURLcode res = curl_easy_perform(easy_handle);
    /* Check for errors */
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}