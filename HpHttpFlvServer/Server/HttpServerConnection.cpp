#include "HttpServerConnection.h"
#include "BoostServer.h"
#include "../Utils/Log.h"

HttpServerConnection::HttpServerConnection(BoostServer* server, tcp::socket& socket)
    : mSession(server->generateSession()),
    mServer(server),
    mSocket(std::move(socket))
{
    LOGI("");

}
HttpServerConnection::~HttpServerConnection() {
    LOGI("");
    if (fp) {
        fclose(fp);
        fp = nullptr;
    }
    this->mSocket.close();
    this->mTimer.cancel();
}
std::string HttpServerConnection::getSession() {
    return mSession;
}

void HttpServerConnection::run()
{
    //异步读取，触发请求的时候把数据写入mHttpRequest
    http::async_read(
        mSocket,
        mTempBuffer,
        mHttpRequest,
        [this](beast::error_code ec,
            std::size_t bytes_transferred)
        {

            boost::ignore_unused(bytes_transferred);
            if (ec) {
                //接收可读数据失败
                LOGE("run error,msg=%s", ec.message().data());
                m_disconnectionCallback(m_arg, mSession);
            }
            else {
                //读取到数据后开始处理
                this->handle();
            }
        });

    /*
    mTimer.async_wait(
        [this](beast::error_code ec)
        {
            if (ec)
            {
            }
        }
    );*/
}
void HttpServerConnection::handle()
{
    //设置Http响应的版本和服务类型
    mHttpResponse.version(mHttpRequest.version());
    mHttpResponse.set(http::field::server, "Boost");

    //根据请求的方法作对应处理
    switch (mHttpRequest.method())
    {
    case http::verb::get:
    {
        //判断请求的url是不是test.flv
        if (mHttpRequest.target() == "/test.flv")
        {
            mHttpResponse.result(http::status::ok);
            mHttpResponse.keep_alive(true);
            mHttpResponse.set(http::field::access_control_allow_origin, "*");
            mHttpResponse.set(http::field::content_type, "video/x-flv");
            mHttpResponse.set(http::field::connection, "close");
            mHttpResponse.set(http::field::expires, "-1");
            mHttpResponse.set(http::field::pragma, "no-cache");
            mHttpResponse.content_length(-1);

            //异步响应Http请求
            http::async_write(
                mSocket,
                mHttpResponse,
                [this](beast::error_code ec, std::size_t)
                {
                    if (ec)
                    {
                        //发送失败
                        LOGE("play flv error,msg=%s", ec.message().data());
                        //this->mSocket.shutdown(tcp::socket::shutdown_send, ec);
                        m_disconnectionCallback(m_arg, mSession);
                    }
                    else {
                        //发送成功
                        LOGI("play flv success");
                        //const char* filename = "../data/test.flv";
                        const char* filename = "../data/笑傲江湖天地作合.flv";

                        //读取flv文件
                        fp = fopen(filename, "rb");
                        //往HTTP请求里一直写入文件内容
                        this->keepWrite();

                    }
                });
        }
        else
        {
            mHttpResponse.result(http::status::ok);
            mHttpResponse.keep_alive(false);
            mHttpResponse.result(http::status::not_found);
            mHttpResponse.set(http::field::content_type, "text/plain");
            beast::ostream(mHttpResponse.body()) << "File not found\r\n";

            http::async_write(
                mSocket,
                mHttpResponse,
                [this](beast::error_code ec, std::size_t)
                {
                    if (ec)
                    {
                        //发送失败
                        LOGE("http::async_write error,msg=%s", ec.message().data());
                        //this->mSocket.shutdown(tcp::socket::shutdown_send, ec
                    }
                    else {
                        //发送成功
                        LOGI("http::async_write success,msg=%s", ec.message().data());
                    }
                    m_disconnectionCallback(m_arg, mSession);
                });
        }
        break;
    }
    default:
    {
        mHttpResponse.result(http::status::bad_request);
        mHttpResponse.set(http::field::content_type, "text/plain");
        beast::ostream(mHttpResponse.body())
            << "Invalid request-method '"
            << std::string(mHttpRequest.method_string())
            << "'";
        mHttpResponse.content_length(mHttpResponse.body().size());

        http::async_write(
            mSocket,
            mHttpResponse,
            [this](beast::error_code ec, std::size_t)
            {
                if (ec)
                {
                    //发送失败
                    LOGE("http::async_write error,msg=%s", ec.message().data());
                    //this->mSocket.shutdown(tcp::socket::shutdown_send, ec);
                }
                else {
                    //发送成功
                    LOGI("http::async_write success,msg=%s", ec.message().data());
                }
                m_disconnectionCallback(m_arg, mSession);
            });
        break;
    }

    }

}
void HttpServerConnection::keepWrite() {
    char data[5000];
    int  size;
    //每次读取5000字节
    size = fread(data, 1, sizeof(data), fp);
    if (size > 0) {
        boost::asio::async_write(
            mSocket,
            boost::asio::buffer(data, size),
            [this](beast::error_code ec, std::size_t)
            {
                if (ec)
                {
                    //发送失败
                    LOGE("keepWrite error,msg=%s", ec.message().data());
                    m_disconnectionCallback(m_arg, mSession);
                }
                else {
                    //发送成功，就继续写入
                    //LOGI("keepWrite successs");
                    this->keepWrite();
                }

            });
    }
    else {
        //文件读取完后，断开连接
        LOGE("keepWrite error,msg= flv buffer finish");
        m_disconnectionCallback(m_arg, mSession);
    }

}


