#include <iostream>

#include <phase2/Http.hpp>

int main(void)
{
    using namespace phase2;

    std::size_t body_start;
    HttpRequestHeader request1{
        "POST / HTTP/1.1\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 168\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Accept-Language: en-US\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:96.0) Gecko/20100101 Firefox/96.0\r\n"
        "\r\n"
        "アスナ！クライン！頼む、十秒だけ持ちこたえてくれ！\n\0\0\0\0"
        "わかった\0\0\0\0\0\0\0\0\0\n"
        "よし　いいぞ\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "スイッチ\r\r\r\r\r\r\r\r\r\r\r\n\0\0\0\0\0\0",
        body_start,
    };
    if (!request1 || body_start != 221)
        std::cerr << "HttpRequestHeader test1 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpRequestHeader test1 success\n\n"
                  << request1;

    HttpRequestHeader request2 = HttpRequestHeader{request1.serialize(), body_start};
    if (!request2 || request1 != request2 || body_start != 221)
        std::cerr << "HttpRequestHeader test2 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpRequestHeader test2 success\n\n"
                  << request2;

    HttpRequestHeader request3{"GET /starburst_stream HTTP/1.1\r\n\r\n", body_start};
    if (!request3 || body_start != 34)
        std::cerr << "HttpRequestHeader test3 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpRequestHeader test3 success\n\n"
                  << request3;

    HttpResponseHeader response1{
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 16\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Server: Apache\r\n"
        "\r\n"
        "starburst stream",
        body_start,
    };
    if (!response1 || body_start != 96)
        std::cerr << "HttpResponseHeader test1 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpResponseHeader test1 success\n\n"
                  << response1;

    HttpResponseHeader response2{response1.serialize(), body_start};
    if (!response2 || response1 != response2 || body_start != 96)
        std::cerr << "HttpResponseHeader test2 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpResponseHeader test2 success\n\n"
                  << response2;

    HttpResponseHeader response3{"HTTP/1.1 404 Not Found\r\n\r\n", body_start};
    if (!response3 || body_start != 26)
        std::cerr << "HttpResponseHeader test3 failed, body_start = " << body_start << "\n";
    else
        std::cerr << "HttpResponseHeader test3 success\n\n"
                  << response3;

    return 0;
}
