#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <libpq-fe.h>
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
    void send_response(std::string response, std::string token) {
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

    // Чтение запроса
    void read_request() {
        http::async_read(socket_, buffer_, request_,
            [self = shared_from_this()](boost::beast::error_code ec, std::size_t) {
                if (!ec) {
                    self->process_request();
                }
            });
    }

    // Обработка запроса
    void process_request() {
        const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 hostaddr=127.0.0.1 port=5432";
        std::string method = request_.method_string().to_string();
        std::string target = request_.target().to_string();
        std::string body = request_.body();
        auto json_body = nlohmann::json::parse(body);
        std::cout << "Received JSON: " << json_body.dump() << std::endl;
        std::string token = "12345";

        if (method == "GET") {
            if (target == "checkToken") {
                send_response("/checkToken", token);
            }
            else if (target == "/getCurrentDeals") {
                PGconn* conn = PQconnectdb(conninfo);
                const char* user_info[1] = {
                    "active"
                    };

                std::vector<std::vector<std::string>> deals_list = getCurrentDeals(conn, user_info);
                PQfinish(conn);
                send_response("User already exists.", token);
            }
            else if (target == "/getMyDeals") {
                send_response("getMyDeals", token);
            }
            else if (target == "/getCurrencyQuotes") {
                send_response("getCurrencyQuotes", token);
            }
            else {
                send_response("Unsupported path for GET request", token);
            }
        }
        else if (method == "POST") {
            if (target == "/signup") {
                PGconn* conn = PQconnectdb(conninfo);
                const char* user_info[6] = {
                    json_body["first_name"].get<std::string>().c_str(), 
                    json_body["second_name"].get<std::string>().c_str(),
                    "0",
                    "0",
                    json_body["login"].get<std::string>().c_str(), 
                    json_body["password"].get<std::string>().c_str()
                    };

                if (!isUserExists(conn, user_info[4])) {
                    addUser(conn, user_info);
                    send_response("Успешная регистрация", token);
                } else {
                    send_response("User already exists.", token);
                }
                PQfinish(conn);
            }
            else if (target == "/login") {
                PGconn* conn = PQconnectdb(conninfo);
                const char* user_info[2] = {
                    json_body["login"].get<std::string>().c_str(), 
                    json_body["password"].get<std::string>().c_str()
                    };

                if (checkLoginAndPassword(conn, user_info)) {
                    send_response("Успешная авторизация", token);
                } else {
                    send_response("Неправильный логин или пароль", token);
                }
                PQfinish(conn);
            }
            else if (target == "/makeDeal") {
                PGconn* conn = PQconnectdb(conninfo);
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
            else if (target == "/cancelDeal") {
                send_response("/cancelDeal", token);
            }
            else {
                send_response("Unsupported path for POST request", token);
            }
        }
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

    bool isUserExists(PGconn* conn, const char* login) {
        const char* PG_is_user_exists = "SELECT 1 FROM users WHERE login = $1 LIMIT 1";
        const char* paramValues[1] = {login};
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

    int addUser(PGconn* conn, const char* user_info[]) {
        const char* PG_add_user = "INSERT INTO users (first_name, second_name, balance_usd, balance_rub, login, password) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_add_user, 6, NULL, user_info, NULL, NULL, 0);

        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
            std::cerr << "Error adding user: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return 1;
        }

        PQclear(result);
        std::cout << "User added successfully!" << std::endl;
        return 0;
    }

    bool checkLoginAndPassword(PGconn* conn, const char* user_info[]) {
        const char* PG_check_login_and_password = "SELECT 1 FROM users WHERE login = $1 AND password = $2";
        PGresult* result = PQexecParams(conn, PG_check_login_and_password, 2, NULL, user_info, NULL, NULL, 0);

        if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cerr << "Error checking user existence: " << PQresultErrorMessage(result) << std::endl;
            PQclear(result);
            return false;
        }

        bool exists = (PQntuples(result) == 1);
        PQclear(result);
        return exists;
    }

    void makeDeal(PGconn* conn, const char* user_info[]) {
        const char* PG_make_deal = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status) VALUES ($1, $2, $3, $4, $5, $6)";
        PGresult* result = PQexecParams(conn, PG_make_deal, 6, NULL, user_info, NULL, NULL, 0);
        PQclear(result);
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