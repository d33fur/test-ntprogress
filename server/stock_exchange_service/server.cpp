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
    MySession(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        std::cout << "LogFunc: start() NEW PROCESSING" << std::endl;

        reset();
        read_request();
    }

private:
    tcp::socket socket_;

    boost::beast::flat_buffer buffer_;

    http::request<http::string_body> request_;

    void reset() {
        std::cout << "LogFunc: reset()" << std::endl;

        buffer_.consume(buffer_.size());
        request_ = {};
    }

    void send_response(std::string response) {

    std::cout << "LogFunc: send_response()" << std::endl;

    auto response_ptr = std::make_shared<std::string>(std::move(response));

    http::response<http::string_body> res;
    
    res.version(request_.version());
    res.result(http::status::ok);
    res.set(http::field::server, "stock_exchange_backend");
    res.set(http::field::content_type, "application/json");
    res.body() = *response_ptr;
    res.prepare_payload();

    http::async_write(socket_, res,
        [self = shared_from_this(), response_ptr](boost::beast::error_code ec, std::size_t) {
            self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        });

    }

    void read_request() {
        std::cout << "LogFunc: read_request()" << std::endl;

        http::async_read(socket_, buffer_, request_,
            [self = shared_from_this()](boost::beast::error_code ec, std::size_t) {
                if (!ec) {
                    self->process_request();
                }
            });
    }

    void process_request() {
        std::cout << "LogFunc: process_request()" << std::endl;

        std::string method = request_.method_string().to_string();
        std::string target = request_.target().to_string();
        std::cout << method << ' ' << target << std::endl;
        bool valid_request;
        logReceivedJson(method);
        if (method == "GET") {
            handle_get_request(target);
        }
        else if (method == "POST") {
            handle_post_request(target);
        }
    }

    void handle_get_request(const std::string& target) {
        std::cout << "LogFunc: handle_get_request()" << std::endl;
        if (target == "/checkToken") {
            checkToken_request();
        }
        else if (target == "/getCurrentDeals" && isTokenValid()) {
            getCurrentDeals_request();
        }
        else if (target == "/getMyDeals" && isTokenValid()) {
            getMyDeals_request();
        }
        else if (target == "/getCurrencyQuotes" && isTokenValid()) {
            getCurrencyQuotes_request();
        }
        else if (target == "/getMyInfo" && isTokenValid()) {
            getMyInfo_request();
        }
        else {
            std::cout << "Log: Unsupported path for GET request or invalid Token" << std::endl;
            send_response("{\"Unsupported path for GET request or invalid Token\"}");
        }
    }


    void handle_post_request(const std::string& target) {
        std::cout << "LogFunc: handle_post_request()" << std::endl;
        if (target == "/signup") {
            signup_request();
        }
        else if (target == "/login") {
            login_request();
        }
        else if (target == "/makeDeal" && isTokenValid()) {
            makeDeal_request();
        }
        else if (target == "/cancelDeal" && isTokenValid()) {
            cancelDeal_request();
        }
        else {
            std::cout << "Log: Unsupported path for POST request or invalid token" << std::endl;
            send_response("{\"Unsupported path for POST request or invalid token\"}");
        }
    }

    void checkToken_request() {
        std::cout << "LogFunc: checkToken_request()" << std::endl;

        auto json_token = request_.find("Token");
        std::string token_from_user(json_token->value());
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());

        bool valid_user = checkToken(user_id, token_from_user);

        nlohmann::json json_response = {
                {"is_user_valid", std::to_string(valid_user)}
            };

        send_response(json_response.dump());
    }

    void getCurrentDeals_request() {
        std::cout << "LogFunc: getCurrentDeals_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        const char* user_info[1] = {
            "active"
        };

        nlohmann::json deals_json = getCurrentDeals(conn, user_info);

        PQfinish(conn);
        send_response(deals_json.dump());
    }

    void getMyDeals_request() {
        std::cout << "LogFunc: getMyDeals_request()" << std::endl;

        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        const char* user_info[1] = {
            user_id.c_str()
        };

        nlohmann::json deals_json = getMyDeals(conn, user_info);

        PQfinish(conn);
        send_response(deals_json.dump());
    }

    void getCurrencyQuotes_request() {
        std::cout << "LogFunc: getCurrencyQuotes_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        nlohmann::json quotes_json = getCurrencyQuotes(conn);

        PQfinish(conn);
        send_response(quotes_json.dump());
    }

    void getMyInfo_request() {
        std::cout << "LogFunc: getMyInfo_request()" << std::endl;

        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        const char* user_info[1] = {
            user_id.c_str()
        };

        nlohmann::json user_info_json = getMyInfo(conn, user_info);

        PQfinish(conn);
        send_response(user_info_json.dump());
    }

    void makeDeal_request() {
        std::cout << "LogFunc: makeDeal_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);

        const char* user_info[6] = {
            user_id.c_str(),
            "0",
            json_body["deal_type"].get<std::string>().c_str(),
            json_body["amount"].get<std::string>().c_str(),
            json_body["exchange_rate"].get<std::string>().c_str(),
            "active"
        };

        makeDeal(conn, user_info);

        PQfinish(conn);

        makeMoneyTransactions();
    }

    void cancelDeal_request() {
        std::cout << "LogFunc: cancelDeal_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);

        const char* deal_info[3] = {
            json_body["deal_id"].get<std::string>().c_str(),
            "cancelled",
            user_id.c_str()
        };

        cancelDeal(conn, deal_info);

        PQfinish(conn);
    }

    void signup_request() {
        std::cout << "LogFunc: signup_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);

        const char* user_info[6] = {
            json_body["first_name"].get<std::string>().c_str(), 
            json_body["second_name"].get<std::string>().c_str(),
            "0",
            "0",
            json_body["login"].get<std::string>().c_str(), 
            json_body["password"].get<std::string>().c_str()
        };

        if (!isUserExists(conn, user_info[4])) {
            std::string user_id = addUserAndGetId(conn, user_info);
            std::string generated_token = generateToken(user_id);

            nlohmann::json json_response = {
                {"user_id", user_id},
                {"token", generated_token}
            };

            send_response(json_response.dump());
        } 
        else {
            send_response("{\"User already exists\"}");
        }

        PQfinish(conn);
    }

    void login_request() {
        std::cout << "LogFunc: login_request()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);

        const char* user_info[2] = {
            json_body["login"].get<std::string>().c_str(), 
            json_body["password"].get<std::string>().c_str()
        };
        
        if (checkLoginAndPassword(conn, user_info)) {
            std::string user_id = getIdWithLogin(conn, json_body["login"].get<std::string>().c_str());
            std::string generated_token = generateToken(user_id);

            nlohmann::json json_response = {
                {"user_id", user_id},
                {"token", generated_token}
            };  
            send_response(json_response.dump());
        } 
        else {
            send_response("{\"Wrong login or password\"}");
        }

        PQfinish(conn);
    }

    void makeMoneyTransactions() {
        std::cout << "LogFunc: makeMoneyTransactions()" << std::endl;

        const char* PG_get_max_buy_deal = "SELECT * FROM deals WHERE deal_type = 'buy' AND deal_status = 'active' ORDER BY exchange_rate DESC, time_start ASC";
        const char* PG_get_min_sell_deals = "SELECT * FROM deals WHERE deal_type = 'sell' AND deal_status = 'active' ORDER BY exchange_rate DESC";

        nlohmann::json deals_max_buy_json = getMaxMinDeals(PG_get_max_buy_deal);
        nlohmann::json deals_min_sell_json = getMaxMinDeals(PG_get_min_sell_deals);
        
        nlohmann::json new_deals_json = {};

        for(int i = 0; i < deals_max_buy_json.size(); i++) {
            for(int j = 0; j < deals_min_sell_json.size(); j++) {
                if(deals_min_sell_json[j]["deal_status"] == "active" && deals_min_sell_json[j]["from_user_id"] != deals_max_buy_json[i]["from_user_id"]) {
                    int max_buy_exchange_rate = std::stoi(deals_max_buy_json[i]["exchange_rate"].get<std::string>());
                    int min_sell_exchange_rate = std::stoi(deals_min_sell_json[j]["exchange_rate"].get<std::string>());

                    if(max_buy_exchange_rate >= min_sell_exchange_rate) {
                        int max_buy_amount = std::stoi(deals_max_buy_json[i]["amount"].get<std::string>());
                        int min_sell_amount = std::stoi(deals_min_sell_json[j]["amount"].get<std::string>());

                        if(max_buy_amount > min_sell_amount) {
                            deals_max_buy_json[i]["amount"] = std::to_string(max_buy_amount - min_sell_amount);
                            deals_min_sell_json[j]["deal_status"] = "done_new";
                            deals_min_sell_json[j]["to_user_id"] = deals_max_buy_json[i]["from_user_id"];

                            nlohmann::json new_deal_row = {
                                {"amount", deals_min_sell_json[j]["amount"]},
                                {"deal_status", "done_new"},
                                {"deal_type", "buy"},
                                {"exchange_rate", deals_min_sell_json[j]["exchange_rate"]},
                                {"from_user_id", deals_max_buy_json[i]["from_user_id"]},
                                {"time_start", deals_max_buy_json[i]["time_start"]},
                                {"to_user_id", deals_min_sell_json[j]["from_user_id"]}
                            };

                            new_deals_json.push_back(new_deal_row);
                        }
                        else if(max_buy_amount == min_sell_amount) {
                            deals_min_sell_json[j]["deal_status"] = "done_new";
                            deals_min_sell_json[j]["to_user_id"] = deals_max_buy_json[i]["from_user_id"];

                            deals_max_buy_json[i]["deal_status"] = "done_new";
                            deals_max_buy_json[i]["to_user_id"] = deals_min_sell_json[j]["from_user_id"];
                        }
                        else {
                            deals_min_sell_json[j]["amount"] = std::to_string(min_sell_amount - max_buy_amount);
                            deals_max_buy_json[i]["deal_status"] = "done_new";
                            deals_max_buy_json[i]["to_user_id"] = deals_min_sell_json[j]["from_user_id"];

                            nlohmann::json new_deal_row = {
                                {"amount", deals_max_buy_json[i]["amount"]},
                                {"deal_status", "done_new"},
                                {"deal_type", "sell"},
                                {"exchange_rate", deals_min_sell_json[j]["exchange_rate"]},
                                {"from_user_id", deals_min_sell_json[j]["from_user_id"]},
                                {"time_start", deals_min_sell_json[j]["time_start"]},
                                {"to_user_id", deals_max_buy_json[i]["from_user_id"]}
                            };

                            new_deals_json.push_back(new_deal_row);
                        }
                    }
                }

                if(deals_max_buy_json[i]["deal_status"] == "done_new") {
                    break;
                }
            }
        }

        updateDealsInfo(deals_max_buy_json);
        updateDealsInfo(deals_min_sell_json);
        updateDealsInfo(new_deals_json);

        updateUserInfo();
    }

    void updateDealsInfo(nlohmann::json deals_json) {
        std::cout << "LogFunc: updateDealsInfo()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        for(auto it : deals_json) {
            if(it.find("id") != it.end()) {
                const char* deal_info[6] = {
                    it["id"].get<std::string>().c_str(),
                    it["from_user_id"].get<std::string>().c_str(),
                    it["to_user_id"].get<std::string>().c_str(),
                    it["amount"].get<std::string>().c_str(),
                    it["deal_status"].get<std::string>().c_str(),
                    it["exchange_rate"].get<std::string>().c_str()
                };

                if(it["deal_status"] == "done_new") {
                    const char* PG_update_deal_info = "UPDATE deals SET to_user_id = $3, amount = $4, deal_status = $5, exchange_rate = $6, time_end = NOW() WHERE id = $1 AND from_user_id = $2";

                    PGresult* result = PQexecParams(conn, PG_update_deal_info, 6, NULL, deal_info, NULL, NULL, 0);
                    PQclear(result);
                }
                else {
                    const char* PG_update_deal_info = "UPDATE deals SET amount = $4 WHERE id = $1 AND from_user_id = $2";

                    PGresult* result = PQexecParams(conn, PG_update_deal_info, 6, NULL, deal_info, NULL, NULL, 0);
                    PQclear(result);
                }
            }
            else {
                const char* deal_info[7] = {
                    it["from_user_id"].get<std::string>().c_str(),
                    it["to_user_id"].get<std::string>().c_str(),
                    it["deal_type"].get<std::string>().c_str(),
                    it["amount"].get<std::string>().c_str(),
                    it["exchange_rate"].get<std::string>().c_str(),
                    it["deal_status"].get<std::string>().c_str(),
                    it["time_start"].get<std::string>().c_str()
                };

                const char* PG_update_deal_info = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status, time_start, time_end) VALUES ($1, $2, $3, $4, $5, $6, $7, NOW())";
                
                PGresult* result = PQexecParams(conn, PG_update_deal_info, 7, NULL, deal_info, NULL, NULL, 0);
                PQclear(result);
            }
        }

        PQfinish(conn);
    }

    void updateUserInfo() {
        std::cout << "LogFunc: updateUserInfo()" << std::endl;
        
        const char* PG_get_done_new_deals = "SELECT * FROM deals WHERE deal_status = 'done_new'";
        nlohmann::json done_new_deals_json = getMaxMinDeals(PG_get_done_new_deals);

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        for(auto it : done_new_deals_json) {
            int balance_rub = std::stoi(it["exchange_rate"].get<std::string>()) * std::stoi(it["amount"].get<std::string>());
            const char* user_info[3] = {
                it["from_user_id"].get<std::string>().c_str(),
                std::to_string(balance_rub).c_str(),
                it["amount"].get<std::string>().c_str()
            };
            if(it["deal_type"] == "sell") {
                const char* PG_update_user_info = "UPDATE users SET balance_rub = balance_rub + $2, balance_usd = balance_usd - $3 WHERE id = $1";

                PGresult* result = PQexecParams(conn, PG_update_user_info, 3, NULL, user_info, NULL, NULL, 0);
                PQclear(result);
            }
            else {
                const char* PG_update_user_info = "UPDATE users SET balance_rub = balance_rub - $2, balance_usd = balance_usd + $3 WHERE id = $1";

                PGresult* result = PQexecParams(conn, PG_update_user_info, 3, NULL, user_info, NULL, NULL, 0);
                PQclear(result);
            }
        }

        const char* PG_update_deals_info = "UPDATE deals SET deal_status = 'done' WHERE deal_status = 'done_new'";
        PGresult* result = PQexec(conn, PG_update_deals_info);

        PQclear(result);
        PQfinish(conn);
        std::cout << "LogFunc: updateUserInfo() END" << std::endl;
    }

    nlohmann::json getMaxMinDeals(const char* PG_get_max_min_buy_sell_deal) {
        std::cout << "LogFunc:  getMaxMinDeals()" << std::endl;

        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);

        PGresult* result = PQexec(conn, PG_get_max_min_buy_sell_deal);

        int rows = PQntuples(result);
        int cols = PQnfields(result);
        nlohmann::json deals_max_buy_json = {};

        for(int row = 0; row < rows; row++) {
            nlohmann::json deal_row = {};

            for(int col = 0; col < cols; col++) {
                const char* column_name = PQfname(result, col);
                deal_row[std::string(column_name)] = std::string(PQgetvalue(result, row, col));
            }
            deals_max_buy_json.push_back(deal_row);
        }

        PQclear(result);
        PQfinish(conn);

        return deals_max_buy_json;
    }

    bool isTokenValid() {
        std::cout << "LogFunc: isTokenValid()" << std::endl;

        auto json_token = request_.find("Token");
        std::string token_from_user(json_token->value());
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());

        return checkToken(user_id, token_from_user);
    }

    bool checkToken(const std::string& user_id, const std::string& token_from_user) {
        std::cout << "LogFunc: checkToken()" << std::endl;

        redisContext* context = redisConnect("127.0.0.1", 6379);
        if (context != NULL && context->err) {
            std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
            return 0;
        }

        std::string key = "user:" + user_id + ":token";
        redisReply* reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());

        std::string token_from_redis(reply->str, reply->len);

        freeReplyObject(reply);
        redisFree(context);

        return token_from_user == token_from_redis ? 1 : 0;
    }

    std::string generateToken(const std::string& user_id) {
        std::cout << "LogFunc: generateToken()" << std::endl;

        redisContext* context = redisConnect("127.0.0.1", 6379);
        if (context != NULL && context->err) {
            std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
            return "error";
        }

        std::string key = "user:" + user_id + ":token";
        std::string token = generateRandomString(128);

        redisReply* reply = (redisReply*)redisCommand(context, "SET %s %s", key.c_str(), token.c_str());

        freeReplyObject(reply);
        redisFree(context);

        return token;
    }

    std::string generateRandomString(size_t length) {
        std::cout << "LogFunc: generateRandomString()" << std::endl;

        const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::random_device rd;
        std::mt19937 generator(rd());

        std::uniform_int_distribution<std::size_t> distribution(0, characters.size() - 1);
        std::string randomString;
        randomString.reserve(length);

        for (std::size_t i = 0; i < length; ++i) {
            randomString.push_back(characters[distribution(generator)]);
        }

        return randomString;
    }

    std::string getIdWithLogin(PGconn* conn, const char* login) {
        std::cout << "LogFunc: getIdWithLogin()" << std::endl;

        const char* PG_get_user_id = "SELECT id FROM users WHERE login = $1";
        const char* user_info_id[1] = {
            login
        };

        PGresult* result = PQexecParams(conn, PG_get_user_id, 1, NULL, user_info_id, NULL, NULL, 0);
        std::string user_id(PQgetvalue(result, 0, 0));

        PQclear(result);

        return user_id;
    }

    nlohmann::json getCurrentDeals(PGconn* conn, const char* deal_status[]) {
        std::cout << "LogFunc: getCurrentDeals()" << std::endl;

        const char* PG_get_current_deals = "SELECT * FROM deals WHERE deal_status = $1";
        PGresult* result = PQexecParams(conn, PG_get_current_deals, 1, NULL, deal_status, NULL, NULL, 0);

        int rows = PQntuples(result);
        int cols = PQnfields(result);
        nlohmann::json deals_json = {};

        for(int row = 0; row < rows; row++) {
            nlohmann::json deal_row = {};

            for(int col = 0; col < cols-1; col++) {
                const char* column_name = PQfname(result, col);
                const char* column_value = PQgetvalue(result, row, col);

                deal_row[std::string(column_name)] = std::string(column_value);
            }
            deals_json.push_back(deal_row);
        }

        PQclear(result);

        return deals_json;
    }

    nlohmann::json getMyDeals(PGconn* conn, const char* deal_status[]) {
        std::cout << "LogFunc: getMyDeals()" << std::endl;

        const char* PG_get_current_deals = "SELECT * FROM deals WHERE from_user_id = $1 OR to_user_id = $1";
        PGresult* result = PQexecParams(conn, PG_get_current_deals, 1, NULL, deal_status, NULL, NULL, 0);

        int rows = PQntuples(result);
        int cols = PQnfields(result);
        nlohmann::json deals_json = {};

        for(int row = 0; row < rows; row++) {
            nlohmann::json deal_row = {};

            for(int col = 0; col < cols; col++) {
                const char* column_name = PQfname(result, col);
                const char* column_value = PQgetvalue(result, row, col);

                deal_row[std::string(column_name)] = std::string(column_value);
            }
            deals_json.push_back(deal_row);
        }

        PQclear(result);

        return deals_json;
    }

    nlohmann::json getCurrencyQuotes(PGconn* conn) {
        std::cout << "LogFunc: getMyDeals()" << std::endl;

        const char* PG_get_currency_quotes = "SELECT exchange_rate, time_start FROM deals";
        PGresult* result = PQexec(conn, PG_get_currency_quotes);

        int rows = PQntuples(result);
        int cols = PQnfields(result);
        std::vector<std::map<std::string, std::string>> currency_quotes_vector;

        for (int row = 0; row < rows; row++) {
            std::map<std::string, std::string> currency_quotes_row;

            for (int col = 0; col < cols; col++) {
                const char* column_name = PQfname(result, col);
                const char* column_value = PQgetvalue(result, row, col);

                currency_quotes_row[std::string(column_name)] = std::string(column_value);
            }
            currency_quotes_vector.push_back(currency_quotes_row);
        }

        PQclear(result);

        std::sort(currency_quotes_vector.begin(), currency_quotes_vector.end(),
          [](const auto& lhs, const auto& rhs) {
              if (lhs.at("time_start") == rhs.at("time_start")) {
                  return std::stod(lhs.at("exchange_rate")) > std::stod(rhs.at("exchange_rate"));
              }

              return lhs.at("time_start") > rhs.at("time_start");
          });

        nlohmann::json currency_quotes_json = currency_quotes_vector;

        return currency_quotes_json;
    }

    nlohmann::json getMyInfo(PGconn* conn, const char* user_id[]) {
        std::cout << "LogFunc: getMyInfo()" << std::endl;

        const char* PG_get_user_info = "SELECT * FROM users WHERE id = $1";
        PGresult* result = PQexecParams(conn, PG_get_user_info, 1, NULL, user_id, NULL, NULL, 0);

        int cols = PQnfields(result);
        nlohmann::json deal_row = {};

        for(int col = 1; col < cols-2; col++) {
            const char* column_name = PQfname(result, col);
            const char* column_value = PQgetvalue(result, 0, col);

            deal_row[std::string(column_name)] = std::string(column_value);
        }

        PQclear(result);

        return deal_row;
    }

    bool isUserExists(PGconn* conn, const char* login) {
        std::cout << "LogFunc: isUserExists()" << std::endl;

        const char* PG_is_user_exists = "SELECT 1 FROM users WHERE login = $1 LIMIT 1";
        const char* paramValues[1] = {
            login
        };

        PGresult* result = PQexecParams(conn, PG_is_user_exists, 1, NULL, paramValues, NULL, NULL, 0);

        bool exists = (PQntuples(result) > 0);

        PQclear(result);

        return exists;
    }

    std::string addUserAndGetId(PGconn* conn, const char* user_info[]) {
        std::cout << "LogFunc: addUserAndGetId()" << std::endl;

        const char* PG_add_user = "INSERT INTO users (first_name, second_name, balance_usd, balance_rub, login, password) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_add_user, 6, NULL, user_info, NULL, NULL, 0);

        PQclear(result);

        const char* PG_get_user_id = "SELECT id FROM users WHERE login = $1";
        const char* user_info_id[1] = {
            user_info[4]
        };

        result = PQexecParams(conn, PG_get_user_id, 1, NULL, user_info_id, NULL, NULL, 0);
        std::string user_id(PQgetvalue(result, 0, 0));

        PQclear(result);

        return user_id;
    }

    bool checkLoginAndPassword(PGconn* conn, const char* user_info[]) {
        std::cout << "LogFunc: checkLoginAndPassword()" << std::endl;

        const char* PG_check_login_and_password = "SELECT 1 FROM users WHERE login = $1 AND password = $2";
        PGresult* result = PQexecParams(conn, PG_check_login_and_password, 2, NULL, user_info, NULL, NULL, 0);

        bool user_exists = (PQntuples(result) == 1);

        PQclear(result);

        return user_exists;
    }

    void makeDeal(PGconn* conn, const char* user_info[]) {
        std::cout << "LogFunc: makeDeal()" << std::endl;

        const char* PG_make_deal = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_make_deal, 6, NULL, user_info, NULL, NULL, 0);

        PQclear(result);
        send_response("{\"success\"}");
    }

    void cancelDeal(PGconn* conn, const char* deal_info[]) {
        std::cout << "LogFunc: cancelDeal()" << std::endl;

        const char* PG_cancel_deal = "UPDATE deals SET deal_status = $2, time_end = NOW() WHERE id = $1 AND from_user_id = $3";
        PGresult* result = PQexecParams(conn, PG_cancel_deal, 3, NULL, deal_info, NULL, NULL, 0);

        PQclear(result);
        send_response("{\"success\"}");
    }

    void logReceivedJson(std::string method) {
        std::cout << "LogFunc: logReceivedJson()" << std::endl;

        if(method == "GET") {
            std::cout << "Log: no json"<< std::endl;
        }
        else if (method == "POST") {
            std::string body = request_.body();
            auto json_body = nlohmann::json::parse(body);
            std::cout << "Log: Received JSON: " << json_body.dump() << std::endl;
        }
        
        auto json_token = request_.find("Token");
        std::string token_from_user(json_token->value());
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value());

        std::cout << "Log: Received user_token: " << token_from_user << std::endl;
        std::cout << "Log: Received user_id: " << user_id << std::endl;
        
    }

};
class MyServer {
public:
    MyServer(asio::io_context& io_context, std::uint16_t port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context) {
        accept();
    }

private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    void accept() {
        std::cout << "LogFunc: accept()" << std::endl;
        acceptor_.async_accept(socket_, [this](boost::beast::error_code ec) {
            if (!ec) {
                std::make_shared<MySession>(std::move(socket_))->start();
            }
            
            accept();
        });
    }
};

int main()
{
    try {
        asio::io_context io_context;
        MyServer server(io_context, 8001);
        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}