#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <map>
#include <random>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <libpq-fe.h>
#include <hiredis/hiredis.h>
#include "json.hpp"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace http = boost::beast::http;

class MySession : public std::enable_shared_from_this<MySession> {
public:
    MySession(tcp::socket socket);

    void start();

private:
    void reset();
    void send_response(std::string response);
    void read_request();
    void process_request();
    void handle_get_request(const std::string& target);
    void handle_post_request(const std::string& target);
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

    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
};

#endif // SESSION_H