#include <filesystem>
#include <iostream>

#include <phase2/Http.hpp>
#include <phase2/Mime.hpp>
#include <phase2/Url.hpp>

int main(int argc, char const *argv[])
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
	else if (request1.getHttpVersion() != std::make_pair(1, 1))
		std::cerr << "HttpRequestHeader test1 failed, HTTP version = "
				  << to_string(request1.getHttpVersion()) << '\n';
	else if (request1.getUrl().path() != "/")
		std::cerr << "HttpRequestHeader test1 failed, url path = "
				  << request1.getUrl().path().string() << '\n';
	else if (request1.getHeader("aCCepT-LAnGuaGE").front() != "en-US")
		std::cerr << "HttpRequestHeader test1 failed, Accept-Language = "
				  << request1.getHeader("AcCEpT-lANGuAge").front() << '\n';
	else
		std::cerr << "HttpRequestHeader test1 success\n";

	HttpRequestHeader request2 = HttpRequestHeader{request1.serialize(), body_start};
	if (!request2 || request1 != request2 || body_start != 221)
		std::cerr << "HttpRequestHeader test2 failed, body_start = " << body_start << "\n";
	else if (request2.getHttpVersion() != std::make_pair(1, 1))
		std::cerr << "HttpRequestHeader test2 failed, HTTP version = "
				  << to_string(request2.getHttpVersion()) << '\n';
	else if (request2.getUrl().path() != "/")
		std::cerr << "HttpRequestHeader test2 failed, url path = "
				  << request2.getUrl().path().string() << '\n';
	else if (request1.getType() != HttpRequestHeader::RequestType::POST)
		std::cerr << "HttpRequestHeader test1 failed, request type = "
				  << to_string(request1.getType()) << '\n';
	else
		std::cerr << "HttpRequestHeader test2 success\n";

	HttpRequestHeader request3{"GET /test48763?param1=value1&veryFASTparam2=starburststream HTTP/1.1\r\n\r\n", body_start};
	if (!request3 || body_start != 72)
		std::cerr << "HttpRequestHeader test3 failed, body_start = " << body_start << "\n";
	else if (request3.getUrl().getParam("veryFASTparam2") != "starburststream")
		std::cerr << "HttpRequestHeader test3 failed, veryFASTparam2 = "
				  << request3.getUrl().getParam("veryFASTparam2") << "\n";
	else if (request3.getUrl().path() != "/test48763")
		std::cerr << "HttpRequestHeader test3 failed, url path = "
				  << request3.getUrl().path().string() << '\n';
	else if (request3.getHttpVersion() != std::make_pair(1, 1))
		std::cerr << "HttpRequestHeader test3 failed, HTTP version = "
				  << to_string(request3.getHttpVersion()) << '\n';
	else
		std::cerr << "HttpRequestHeader test3 success\n";

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
		std::cerr << "HttpResponseHeader test1 success\n";

	HttpResponseHeader response2{response1.serialize(), body_start};
	if (!response2 || response1 != response2 || body_start != 96)
		std::cerr << "HttpResponseHeader test2 failed, body_start = " << body_start << "\n";
	else
		std::cerr << "HttpResponseHeader test2 success\n";

	HttpResponseHeader response3{"HTTP/1.1 404 Not Found\r\n\r\n", body_start};
	if (!response3 || body_start != 26)
		std::cerr << "HttpResponseHeader test3 failed, body_start = " << body_start << "\n";
	else
		std::cerr << "HttpResponseHeader test3 success\n";

	std::string mime = get_mime(argv[0]);
	if (mime.compare("application/x-pie-executable") != 0)
		std::cerr << "get_mime test failed\n";
	else
		std::cerr << "get_mime test success\n";

	return 0;
}
