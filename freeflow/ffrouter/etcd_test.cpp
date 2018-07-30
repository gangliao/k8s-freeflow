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

    return 0;
}

TEST(ETCD, GetKeyValue)
{
    CURLcode return_code;
    return_code = curl_global_init(CURL_GLOBAL_SSL);

    EXPECT_EQ(CURLE_OK, return_code) << "init libcurl failed.";

    CURL *easy_handle = curl_easy_init();
    CHECK_NOTNULL(easy_handle);

    char *buff_p = NULL;

    curl_easy_setopt(easy_handle, CURLOPT_URL, "http://127.0.0.1/v2/keys/message1?wait=true");
    curl_easy_setopt(easy_handle, CURLOPT_PORT, 2379);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_data);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, buff_p);

    curl_easy_perform(easy_handle);

    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
}