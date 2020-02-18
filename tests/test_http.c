#include <stdlib.h>
#include <string.h>
#include "include/unity.h"
#include "../src/http.h"
#include "../src/alloc.h"

struct ff_request *mock_test_http_request(char *http_request)
{
    struct ff_request *request = ff_request_alloc();
    request->payload = ff_request_payload_node_alloc();
    request->payload_length = strlen(http_request);
    request->payload->length = strlen(http_request);
    ff_request_payload_load_buff(request->payload, strlen(http_request), http_request);

    return request;
}

void test_http_get_host_empty_reqest()
{
    struct ff_request *request = mock_test_http_request("");

    char *host = ff_http_get_destination_host(request);

    TEST_ASSERT_EQUAL_MESSAGE(NULL, host, "host check failed");

    ff_request_free(request);
    free(host);
}

void test_http_get_host_valid_reqest()
{
    struct ff_request *request = mock_test_http_request("POST / HTTP/1.1\nHost: stackoverflow.com\n\nSome\nTest\nData");

    char *host = ff_http_get_destination_host(request);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("stackoverflow.com", host, "host check failed");

    ff_request_free(request);
    free(host);
}

void test_http_get_host_no_host_header()
{
    struct ff_request *request = mock_test_http_request("POST / HTTP/1.1\nConnection: close\n\nSome\nTest\nData");

    char *host = ff_http_get_destination_host(request);

    TEST_ASSERT_EQUAL_MESSAGE(NULL, host, "host check failed");

    ff_request_free(request);
    free(host);
}

void test_http_get_host_host_in_body()
{
    struct ff_request *request = mock_test_http_request("POST / HTTP/1.1\nSome: header\n\nSome\nTest\nData\nHost: somehost.com");

    char *host = ff_http_get_destination_host(request);

    TEST_ASSERT_EQUAL_MESSAGE(NULL, host, "host check failed");

    ff_request_free(request);
    free(host);
}

void test_http_get_host_multiple_headers()
{
    struct ff_request *request = mock_test_http_request("POST / HTTP/1.1\nSome: header\nOther: header\nHOST: google.com \n\n\nSome\nTest\nData\nHost: somehost.com");

    char *host = ff_http_get_destination_host(request);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("google.com", host, "host check failed");

    ff_request_free(request);
    free(host);
}

void test_http_unencrypted_google()
{
    struct ff_request *request = mock_test_http_request("GET / HTTP/1.1\nConnection: close\nHost: google.com\n\n");

    ff_http_send_request_unencrypted(request);

    TEST_ASSERT_EQUAL_MESSAGE(FF_REQUEST_STATE_SENT, request->state, "state check failed");

    ff_request_free(request);
}
