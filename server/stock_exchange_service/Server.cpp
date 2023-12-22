#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <libpq-fe.h>
#include <hiredis/hiredis.h>
#include <openssl/rand.h>
#include "json.hpp"

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace http = boost::beast::http;

class MySession : public std::enable_shared_from_this<MySession> {
public:
    MySession(tcp::socket socket) : socket_(std::move(socket)) {}

    // Запуск обработки сессии
    void start() {
        read_request();
    }

private:
    // Сокет для передачи данных
    tcp::socket socket_;

    // Буфер для данных
    boost::beast::flat_buffer buffer_;

    // Запрос HTTP
    http::request<http::string_body> request_;

    // Отправка ответа
    void send_response(std::string response) {
        auto response_ptr = std::make_shared<std::string>(std::move(response));

        http::response<http::string_body> res;
        res.version(request_.version());
        res.result(http::status::ok);
        res.set(http::field::server, "MyServer");
        res.set(http::field::content_type, "text/plain");
        res.body() = *response_ptr;
        res.prepare_payload();

        http::async_write(socket_, res,
            [self = shared_from_this(), response_ptr](boost::beast::error_code ec, std::size_t) {
                self->socket_.shutdown(tcp::socket::shutdown_send, ec);
            });
    }

    void read_request() {
        http::async_read(socket_, buffer_, request_,
            [self = shared_from_this()](boost::beast::error_code ec, std::size_t) {
                if (!ec) {
                    self->process_request();
                }
            });
    }

    void process_request() {
        std::string method = request_.method_string().to_string();
        std::string target = request_.target().to_string();
        bool valid_request;
        try {
            isTokenValid();

            if (method == "GET") {
                if (target == "/checkToken") {
                    checkToken_request();
                }
                else if (target == "/getCurrentDeals") {
                    getCurrentDeals_request();
                }
                else if (target == "/getMyDeals") {
                    getMyDeals_request();
                }
                else if (target == "/getCurrencyQuotes") {
                    getCurrencyQuotes_request();
                }
                else {
                    send_response("Unsupported path for GET request");
                }
            }
            else if (method == "POST") {
                if (target == "/makeDeal") {
                    makeDeal_request();
                }
                else if (target == "/cancelDeal") {
                    cancelDeal_request();
                }
                else {
                    send_response("Unsupported path for POST request");
                }
            }
            else {
                send_response("Unsupported method request");
            }
        }
        catch(const std::exception& e) {
            std::cout << "Bad token" << std::endl;
            if (method == "POST") {
                if (target == "/signup") {
                    signup_request();
                }
                else if (target == "/login") {
                    login_request();
                }
                else {
                    send_response("Unsupported path for POST request");
                }
            }
        }
    }

    void signup_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);
        std::cout << "amogus" << std::endl;
        std::string body = request_.body();
        std::cout << "amogus" << std::endl;
        auto json_body = nlohmann::json::parse(body);
        const char* user_info[6] = {
            json_body["first_name"].get<std::string>().c_str(), 
            json_body["second_name"].get<std::string>().c_str(),
            "0",
            "0",
            json_body["login"].get<std::string>().c_str(), 
            json_body["password"].get<std::string>().c_str()
        };
        std::cout << "amogus" << std::endl;
        if (!isUserExists(conn, user_info[4])) {
            std::string user_id = addUserAndGetId(conn, user_info);
            std::string generated_token = generateToken(user_id);
            send_response(user_id + " " + generated_token);
        } else {
            send_response("User already exists.");
        }
        PQfinish(conn);
        std::cout << "amogus" << std::endl;
    }

    void login_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        const char* user_info[2] = {
            json_body["login"].get<std::string>().c_str(), 
            json_body["password"].get<std::string>().c_str()
        };
        
        if (checkLoginAndPassword(conn, user_info)) {
            std::string user_id = GetIdWithLogin(conn, json_body["login"].get<std::string>().c_str());
            std::string generated_token = generateToken(user_id);
            send_response(user_id + " " + generated_token);
        } else {
            send_response("Неправильный логин или пароль");
        }
        PQfinish(conn);
    }

    void checkToken_request() {
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        std::cout << "/checkToken" << std::endl << "Received JSON: " << json_body.dump() << std::endl;
        auto json_token = request_.find("Token");
        std::string token_from_user(json_token->value().data(), json_token->value().length());
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value().data(), json_user_id->value().length());
        bool valid_user = checkToken(user_id, token_from_user);
        send_response("{\"is_user_valid\": \"" + std::to_string(valid_user) + "\"}");
    }

    void getCurrentDeals_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);
        const char* user_info[1] = {
            "active"
        };

        std::vector<std::vector<std::string>> deals_string = getCurrentDeals(conn, user_info); // переделать в json
        PQfinish(conn);
        send_response("current_deals_string");
    }

    void getMyDeals_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value().data(), json_user_id->value().length());
        PGconn* conn = PQconnectdb(conninfo);
        const char* user_info[1] = {
            user_id.c_str()
        };

        std::vector<std::vector<std::string>> deals_string = getMyDeals(conn, user_info); // переделать в json
        PQfinish(conn);
        send_response("my_deals_string");
    }

    void getCurrencyQuotes_request() {
        send_response("getCurrencyQuotes");
    }

    void makeDeal_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        const char* user_info[6] = {
            "1",
            "0",
            json_body["deal_type"].get<std::string>().c_str(),
            json_body["amount"].get<std::string>().c_str(),
            json_body["exchange_rate"].get<std::string>().c_str(),
            "active"
        };
        
        makeDeal(conn, user_info);
        PQfinish(conn);
    }

    void cancelDeal_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        PGconn* conn = PQconnectdb(conninfo);
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        const char* deal_info[2] = {
            json_body["deal_id"].get<std::string>().c_str(),
            "cancelled"
        };
        
        cancelDeal(conn, deal_info);
        PQfinish(conn);
    }

    bool isTokenValid() {
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        std::cout << "/checkToken" << std::endl << "Received JSON: " << json_body.dump() << std::endl;
        auto json_token = request_.find("Token");
        std::string token_from_user(json_token->value().data(), json_token->value().length());
        auto json_user_id = request_.find("User_id");
        std::string user_id(json_user_id->value().data(), json_user_id->value().length());
        return checkToken(user_id, token_from_user);
    }

    bool checkToken(const std::string& user_id, const std::string& token_from_user) {
        redisContext* context = redisConnect("127.0.0.1", 6379);
        if (context != NULL && context->err) {
            std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
            return 0;
        }
        std::string key = "user:" + user_id + ":token";
        redisReply* reply = (redisReply*)redisCommand(context, "GET %s", key.c_str());
        std::string token_from_redis = reply->str;
        // if (reply->str) {
        //     token_from_redis = reply->str;
        // }
        freeReplyObject(reply);
        token_from_user == token_from_redis ? 1 : 0;
    }

    std::string generateToken(const std::string& user_id) {
        redisContext* context = redisConnect("127.0.0.1", 6379);
        if (context != NULL && context->err) {
            std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
            return "error";
        }
        std::string key = "user:" + user_id + ":token";
        std::string token = generateRandomString(128);
        redisReply* reply = (redisReply*)redisCommand(context, "SET %s %s", key.c_str(), token.c_str());
        std::string token_from_redis = reply->str;
        // if (reply->str) {
        //     token_from_redis = reply->str;
        // }
        freeReplyObject(reply);
        return token_from_redis;
    }

    std::string generateRandomString(size_t length) {
        std::string randomString;
        randomString.resize(length);
        RAND_bytes(reinterpret_cast<unsigned char*>(&randomString[0]), static_cast<int>(length));
        return randomString;
    }

    std::string GetIdWithLogin(PGconn* conn, const char* login) {
        const char* PG_get_user_id = "SELECT id FROM users WHERE login = $1";
        const char* user_info_id[1] = {
            login
        };
        PGresult* result = PQexecParams(conn, PG_get_user_id, 1, NULL, user_info_id, NULL, NULL, 0);
        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
            std::cerr << "Error adding user: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return 0;
        }
        std::cout << "GetIdWithLogin!" << std::endl;
        std::string user_id(PQgetvalue(result, 0, 0));
        PQclear(result);
        return user_id;
    }

    std::vector<std::vector<std::string>> getCurrentDeals(PGconn* conn, const char* deal_status[]) {
        const char* PG_get_current_deals = "SELECT * FROM deals WHERE deal_status = $1";
        PGresult* result = PQexecParams(conn, PG_get_current_deals, 1, NULL, deal_status, NULL, NULL, 0);
        int rows = PQntuples(result);
        int cols = PQnfields(result);
        std::vector<std::vector<std::string>> deals_vector(rows);
        for(int row = 0; row < rows; row++) {
            for(int col = 0; col < cols; col++) {
                std::string temp_deal(PQgetvalue(result, row, col));
                deals_vector[row].push_back(temp_deal);
            }
        }
        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cerr << "Error : " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
        }

        PQclear(result);
        return deals_vector;
    }

    std::vector<std::vector<std::string>> getMyDeals(PGconn* conn, const char* deal_status[]) {
        const char* PG_get_current_deals = "SELECT * FROM deals WHERE deal_status = $1";
        PGresult* result = PQexecParams(conn, PG_get_current_deals, 1, NULL, deal_status, NULL, NULL, 0);
        int rows = PQntuples(result);
        int cols = PQnfields(result);
        std::vector<std::vector<std::string>> deals_vector(rows);
        for(int row = 0; row < rows; row++) {
            for(int col = 0; col < cols; col++) {
                std::string temp_deal(PQgetvalue(result, row, col));
                deals_vector[row].push_back(temp_deal);
            }
        }
        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cerr << "Error : " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
        }

        PQclear(result);
        return deals_vector;
    }

    bool isUserExists(PGconn* conn, const char* login) {
        const char* PG_is_user_exists = "SELECT 1 FROM users WHERE login = $1 LIMIT 1";
        const char* paramValues[1] = {
            login
        };
        PGresult* result = PQexecParams(conn, PG_is_user_exists, 1, NULL, paramValues, NULL, NULL, 0);

        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cerr << "Error checking user existence: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return false;
        }

        bool exists = (PQntuples(result) > 0);
        PQclear(result);
        return exists;
    }

    std::string addUserAndGetId(PGconn* conn, const char* user_info[]) {
        const char* PG_add_user = "INSERT INTO users (first_name, second_name, balance_usd, balance_rub, login, password) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_add_user, 6, NULL, user_info, NULL, NULL, 0);

        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
            std::cerr << "Error adding user: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return "error";
        }

        PQclear(result);
        const char* PG_get_user_id = "SELECT id FROM users WHERE login = $1";
        const char* user_info_id[1] = {
            user_info[4]
        };
        result = PQexecParams(conn, PG_get_user_id, 1, NULL, user_info_id, NULL, NULL, 0);
        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
            std::cerr << "Error adding user: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return "error";
        }
        std::cout << "User added successfully!" << std::endl;
        std::string user_id(PQgetvalue(result, 0, 0));
        PQclear(result);
        return user_id;
    }

    bool checkLoginAndPassword(PGconn* conn, const char* user_info[]) {
        const char* PG_check_login_and_password = "SELECT 1 FROM users WHERE login = $1 AND password = $2";
        PGresult* result = PQexecParams(conn, PG_check_login_and_password, 2, NULL, user_info, NULL, NULL, 0);

        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cerr << "Error checking user existence: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return false;
        }

        bool user_exists = (PQntuples(result) == 1);
        PQclear(result);
        return user_exists;
    }

    void makeDeal(PGconn* conn, const char* user_info[]) {
        const char* PG_make_deal = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_make_deal, 6, NULL, user_info, NULL, NULL, 0);
        PQclear(result);
    }

<<<<<<< HEAD
    void cancelDeal(PGconn* conn, const char* deal_info[]) {
        const char* PG_cancel_deal = "UPDATE deals SET deal_status = $2 WHERE id = $1";
        PGresult* result = PQexecParams(conn, PG_cancel_deal, 2, NULL, deal_info, NULL, NULL, 0);
        PQclear(result);
    }
>>>>>>> 52767796ab66eeb4a55f0d72a93f44b8ecf4cfd7



};
public:
    MyServer(asio::io_context& io_context, std::uint16_t port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), socket_(io_context) {
        accept();
    }

private:
    tcp::acceptor acceptor_;
    tcp::socket socket_;

    void accept() {
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