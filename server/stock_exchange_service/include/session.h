#ifndef SESSION_H
#define SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <random>
#include <libpq-fe.h>
#include <hiredis/hiredis.h>
#include "json.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

void http_server(tcp::acceptor& acceptor, tcp::socket& socket);

namespace my_program_state {
  std::size_t request_count();
  std::time_t now();
}

class MySession : public std::enable_shared_from_this<MySession> {
public:
    MySession(tcp::socket socket);

    void start();

private:
    void read_request();
    void check_deadline();
    void process_request();
    void handle_get_request(const std::string& target);
    void handle_post_request(const std::string& target);
    void send_response(std::string response);
    void write_response();
    
    void checkToken_request();
    void getCurrentDeals_request();
    void getMyDeals_request();
    void getCurrencyQuotes_request();
    void getMyInfo_request();
    void makeDeal_request();
    void cancelDeal_request();
    void signup_request();
    void login_request();
    void makeMoneyTransactions();
    void updateDealsInfo(nlohmann::json deals_json);
    void updateUserInfo();  
    nlohmann::json getMaxMinDeals(const char* PG_get_max_min_buy_sell_deal);
    bool isTokenValid();
    bool checkToken(const std::string& user_id, const std::string& token_from_user);
    std::string generateToken(const std::string& user_id);
    std::string generateRandomString(size_t length);
    std::string getIdWithLogin(PGconn* conn, const char* login);
    nlohmann::json getCurrentDeals(PGconn* conn, const char* deal_status[]);
    nlohmann::json getMyDeals(PGconn* conn, const char* deal_status[]);
    nlohmann::json getCurrencyQuotes(PGconn* conn);
    nlohmann::json getMyInfo(PGconn* conn, const char* user_id[]);
    bool isUserExists(PGconn* conn, const char* login);
    std::string addUserAndGetId(PGconn* conn, const char* user_info[]);
    bool checkLoginAndPassword(PGconn* conn, const char* user_info[]);
    void makeDeal(PGconn* conn, const char* user_info[]);
    void cancelDeal(PGconn* conn, const char* deal_info[]);
    void logReceivedJson(std::string method);
    std::string get_request_body();

    tcp::socket socket_;

    beast::flat_buffer buffer_{8192};

    http::request<http::dynamic_body> request_;

    http::response<http::dynamic_body> response_;

    net::steady_timer deadline_{
        socket_.get_executor(), std::chrono::seconds(60)};
};

#endif // SESSION_H