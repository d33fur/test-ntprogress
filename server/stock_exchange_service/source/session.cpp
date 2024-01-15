#include "session.h"

void http_server(tcp::acceptor& acceptor, tcp::socket& socket) {
    std::cout << "LogFunc: http_server()" << std::endl;
    acceptor.async_accept(socket, [&](beast::error_code ec) {
        if(!ec) {
            std::make_shared<MySession>(std::move(socket))->start();
            http_server(acceptor, socket);
        }
    });
}

std::size_t my_program_state::request_count() {
    std::cout << "request_count()" << std::endl;
    static std::size_t count = 0;
    return ++count;
}

std::time_t my_program_state::now() {
    std::cout << "now()" << std::endl;
    return std::time(0);
}

MySession::MySession(tcp::socket socket) : socket_(std::move(socket)) {}

void MySession::start() {
    std::cout << "LogFunc: start() NEW PROCESSING" << std::endl;
    read_request();
    check_deadline();
}

void MySession::read_request() {
    std::cout << "LogFunc: read_request()" << std::endl;
    auto self = shared_from_this();

    http::async_read(socket_, buffer_, request_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if(!ec) {
            self->process_request();
        }
    });
}

void MySession::check_deadline() {
    std::cout << "LogFunc: check_deadline()" << std::endl;
    auto self = shared_from_this();

    deadline_.async_wait([self](beast::error_code ec) {
        if(!ec) {
            self->socket_.close(ec);
        }
    });
}

void MySession::process_request() {
    std::cout << "LogFunc: process_request()" << std::endl;

    response_.version(request_.version());
    response_.keep_alive(false);

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

void MySession::handle_get_request(const std::string& target) {
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

void MySession::handle_post_request(const std::string& target) {
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

void MySession::send_response(std::string content) {
    std::cout << "LogFunc: send_response()" << std::endl;

    response_.version(request_.version());
    response_.result(http::status::ok);
    response_.set(http::field::server, "stock_exchange_backend");
    response_.set(http::field::content_type, "application/json");
    beast::ostream(response_.body()) << content;

    write_response();
}

void MySession::write_response() {
    std::cout << "LogFunc: write_response()" << std::endl;
    auto self = shared_from_this();

    response_.content_length(response_.body().size());

    http::async_write(socket_, response_,
        [self](beast::error_code ec, std::size_t) {
        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        self->deadline_.cancel();
    });
}

void MySession::checkToken_request() {
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

void MySession::getCurrentDeals_request() {
    std::cout << "LogFunc: getCurrentDeals_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    const char* user_info[1] = {
        "active"
    };

    nlohmann::json deals_json = getCurrentDeals(conn, user_info);

    PQfinish(conn);
    send_response(deals_json.dump());
}

void MySession::getMyDeals_request() {
    std::cout << "LogFunc: getMyDeals_request()" << std::endl;

    auto json_user_id = request_.find("User_id");
    std::string user_id(json_user_id->value());

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    const char* user_info[1] = {
        user_id.c_str()
    };

    nlohmann::json deals_json = getMyDeals(conn, user_info);

    PQfinish(conn);
    send_response(deals_json.dump());
}

void MySession::getCurrencyQuotes_request() {
    std::cout << "LogFunc: getCurrencyQuotes_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    nlohmann::json quotes_json = getCurrencyQuotes(conn);

    PQfinish(conn);
    send_response(quotes_json.dump());
}

void MySession::getMyInfo_request() {
    std::cout << "LogFunc: getMyInfo_request()" << std::endl;

    auto json_user_id = request_.find("User_id");
    std::string user_id(json_user_id->value());

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    const char* user_info[1] = {
        user_id.c_str()
    };

    nlohmann::json user_info_json = getMyInfo(conn, user_info);

    PQfinish(conn);
    send_response(user_info_json.dump());
}

void MySession::makeDeal_request() {
    std::cout << "LogFunc: makeDeal_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    auto json_user_id = request_.find("User_id");
    std::string user_id(json_user_id->value());
    std::string body = get_request_body();
    auto json_body = nlohmann::json::parse(body);

    std::string string_values[6] = {
        user_id,
        "0",
        json_body["deal_type"].get<std::string>(),
        json_body["amount"].get<std::string>(),
        json_body["exchange_rate"].get<std::string>(),
        "active"
    };

    const char* user_info[6];
    for(int i = 0; i < 6; i++) {
        user_info[i] = string_values[i].c_str();
    }

    makeDeal(conn, user_info);

    PQfinish(conn);

    makeMoneyTransactions();
}

void MySession::cancelDeal_request() {
    std::cout << "LogFunc: cancelDeal_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    auto json_user_id = request_.find("User_id");
    std::string user_id(json_user_id->value());
    std::string body = get_request_body();
    auto json_body = nlohmann::json::parse(body);

    std::string temp_strings[3] = {
        json_body["deal_id"].get<std::string>(),
        "cancelled",
        user_id
    };

    const char* deal_info[3];
    for(int i = 0; i < 3; i++) {
        deal_info[i] = temp_strings[i].c_str();
    }

    cancelDeal(conn, deal_info);

    PQfinish(conn);
}

void MySession::signup_request() {
    std::cout << "LogFunc: signup_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    std::string body = get_request_body();
    auto json_body = nlohmann::json::parse(body);

    std::string temp_strings[6] = {
        json_body["first_name"].get<std::string>(),
        json_body["second_name"].get<std::string>(),
        "0",
        "0",
        json_body["login"].get<std::string>(),
        json_body["password"].get<std::string>()
    };

    const char* user_info[6];
    for(int i = 0; i < 6; i++) {
        user_info[i] = temp_strings[i].c_str();
    }


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

void MySession::login_request() {
    std::cout << "LogFunc: login_request()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    std::string body = get_request_body();
    auto json_body = nlohmann::json::parse(body);

    std::string temp_strings[2] = {
        json_body["login"].get<std::string>(),
        json_body["password"].get<std::string>()
    };

    const char* user_info[2];
    for(int i = 0; i < 2; i++) {
        user_info[i] = temp_strings[i].c_str();
    }
    
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

void MySession::makeMoneyTransactions() {
    std::cout << "LogFunc: makeMoneyTransactions()" << std::endl;

    const char* PG_get_max_buy_deal = "SELECT * FROM deals WHERE deal_type = 'buy' AND deal_status = 'active' ORDER BY exchange_rate DESC, time_start ASC";
    const char* PG_get_min_sell_deals = "SELECT * FROM deals WHERE deal_type = 'sell' AND deal_status = 'active' ORDER BY exchange_rate DESC, time_start ASC";

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
                        deals_min_sell_json[j]["exchange_rate"] = deals_max_buy_json[i]["exchange_rate"];
                        deals_min_sell_json[j]["to_user_id"] = deals_max_buy_json[i]["from_user_id"];

                        nlohmann::json new_deal_row = {
                            {"amount", deals_min_sell_json[j]["amount"]},
                            {"deal_status", "done_new"},
                            {"deal_type", "buy"},
                            {"exchange_rate", deals_max_buy_json[i]["exchange_rate"]},
                            {"from_user_id", deals_max_buy_json[i]["from_user_id"]},
                            {"time_start", deals_max_buy_json[i]["time_start"]},
                            {"to_user_id", deals_min_sell_json[j]["from_user_id"]}
                        };

                        new_deals_json.push_back(new_deal_row);
                    }
                    else if(max_buy_amount == min_sell_amount) {
                        deals_min_sell_json[j]["deal_status"] = "done_new";
                        deals_min_sell_json[j]["to_user_id"] = deals_max_buy_json[i]["from_user_id"];
                        deals_min_sell_json[j]["exchange_rate"] = deals_max_buy_json[i]["exchange_rate"];

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
                            {"exchange_rate", deals_max_buy_json[i]["exchange_rate"]},
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

void MySession::updateDealsInfo(nlohmann::json deals_json) {
    std::cout << "LogFunc: updateDealsInfo()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    for(auto it : deals_json) {
        if(it.find("id") != it.end()) {
            if(it["deal_status"] == "done_new") {
                std::string temp_strings[6] = {
                    it["id"].get<std::string>(),
                    it["from_user_id"].get<std::string>(),
                    it["to_user_id"].get<std::string>(),
                    it["amount"].get<std::string>(),
                    it["deal_status"].get<std::string>(),
                    it["exchange_rate"].get<std::string>()
                };

                const char* deal_info[6];
                for(int i = 0; i < 6; i++) {
                    deal_info[i] = temp_strings[i].c_str();
                }

                const char* PG_update_deal_info = "UPDATE deals SET to_user_id = $3, amount = $4, deal_status = $5, exchange_rate = $6, time_end = NOW() WHERE id = $1 AND from_user_id = $2";

                PGresult* result = PQexecParams(conn, PG_update_deal_info, 6, NULL, deal_info, NULL, NULL, 0);
                PQclear(result);
            }
            else {
                std::string temp_strings[3] = {
                    it["id"].get<std::string>(),
                    it["from_user_id"].get<std::string>(),
                    it["amount"].get<std::string>()
                };

                const char* deal_info[3];
                for(int i = 0; i < 3; i++) {
                    deal_info[i] = temp_strings[i].c_str();
                }

                const char* PG_update_deal_info = "UPDATE deals SET amount = $3 WHERE id = $1 AND from_user_id = $2";

                PGresult* result = PQexecParams(conn, PG_update_deal_info, 3, NULL, deal_info, NULL, NULL, 0);
                PQclear(result);
            }
        }
        else {
            std::string string_values[7] = {
                it["from_user_id"].get<std::string>(),
                it["to_user_id"].get<std::string>(),
                it["deal_type"].get<std::string>(),
                it["amount"].get<std::string>(),
                it["exchange_rate"].get<std::string>(),
                it["deal_status"].get<std::string>(),
                it["time_start"].get<std::string>()
            };

            const char* deal_info[7];
            for(int i = 0; i < 7; i++) {
                deal_info[i] = string_values[i].c_str();
            }

            const char* PG_update_deal_info = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status, time_start, time_end) VALUES ($1, $2, $3, $4, $5, $6, $7, NOW())";
            
            PGresult* result = PQexecParams(conn, PG_update_deal_info, 7, NULL, deal_info, NULL, NULL, 0);
            PQclear(result);
        }
    }

    PQfinish(conn);
}

void MySession::updateUserInfo() {
    std::cout << "LogFunc: updateUserInfo()" << std::endl;
    
    const char* PG_get_done_new_deals = "SELECT * FROM deals WHERE deal_status = 'done_new' AND deal_type = 'sell'";
    nlohmann::json done_new_deals_json = getMaxMinDeals(PG_get_done_new_deals);

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
    PGconn* conn = PQconnectdb(conninfo);

    for(auto it : done_new_deals_json) {
        int balance_rub = std::stoi(it["exchange_rate"].get<std::string>()) * std::stoi(it["amount"].get<std::string>());
        std::string temp_strings[3] = {
            it["from_user_id"].get<std::string>(),
            std::to_string(balance_rub),
            it["amount"].get<std::string>()
        };

        const char* user_info[3];
        for(int i = 0; i < 3; i++) {
            user_info[i] = temp_strings[i].c_str();
        }

        const char* PG_update_user_info = "UPDATE users SET balance_rub = balance_rub + $2, balance_usd = balance_usd - $3 WHERE id = $1";

        PGresult* result = PQexecParams(conn, PG_update_user_info, 3, NULL, user_info, NULL, NULL, 0);

        user_info[0] = it["to_user_id"].get<std::string>().c_str();

        PG_update_user_info = "UPDATE users SET balance_rub = balance_rub - $2, balance_usd = balance_usd + $3 WHERE id = $1";

        result = PQexecParams(conn, PG_update_user_info, 3, NULL, user_info, NULL, NULL, 0);
        PQclear(result);
    }

    const char* PG_update_deals_info = "UPDATE deals SET deal_status = 'done' WHERE deal_status = 'done_new'";
    PGresult* result = PQexec(conn, PG_update_deals_info);

    PQclear(result);
    PQfinish(conn);
    std::cout << "LogFunc: updateUserInfo() END" << std::endl;
}

nlohmann::json MySession::getMaxMinDeals(const char* PG_get_max_min_buy_sell_deal) {
    std::cout << "LogFunc:  getMaxMinDeals()" << std::endl;

    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=db port=5432";    
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

bool MySession::isTokenValid() {
    std::cout << "LogFunc: isTokenValid()" << std::endl;

    auto json_token = request_.find("Token");
    std::string token_from_user(json_token->value());
    auto json_user_id = request_.find("User_id");
    std::string user_id(json_user_id->value());

    return checkToken(user_id, token_from_user);
}

bool MySession::checkToken(const std::string& user_id, const std::string& token_from_user) {
    std::cout << "LogFunc: checkToken()" << std::endl;

    redisContext* context = redisConnect("redis", 6379);
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

std::string MySession::generateToken(const std::string& user_id) {
    std::cout << "LogFunc: generateToken()" << std::endl;

    redisContext* context = redisConnect("redis", 6379);
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

std::string MySession::generateRandomString(size_t length) {
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

std::string MySession::getIdWithLogin(PGconn* conn, const char* login) {
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

nlohmann::json MySession::getCurrentDeals(PGconn* conn, const char* deal_status[]) {
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

nlohmann::json MySession::getMyDeals(PGconn* conn, const char* deal_status[]) {
    std::cout << "LogFunc: getMyDeals()" << std::endl;

    const char* PG_get_current_deals = "SELECT * FROM deals WHERE from_user_id = $1";
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

nlohmann::json MySession::getCurrencyQuotes(PGconn* conn) {
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

nlohmann::json MySession::getMyInfo(PGconn* conn, const char* user_id[]) {
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

bool MySession::isUserExists(PGconn* conn, const char* login) {
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

std::string MySession::addUserAndGetId(PGconn* conn, const char* user_info[]) {
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

bool MySession::checkLoginAndPassword(PGconn* conn, const char* user_info[]) {
    std::cout << "LogFunc: checkLoginAndPassword()" << std::endl;

    const char* PG_check_login_and_password = "SELECT 1 FROM users WHERE login = $1 AND password = $2";
    PGresult* result = PQexecParams(conn, PG_check_login_and_password, 2, NULL, user_info, NULL, NULL, 0);

    bool user_exists = (PQntuples(result) == 1);

    PQclear(result);

    return user_exists;
}

void MySession::makeDeal(PGconn* conn, const char* user_info[]) {
    std::cout << "LogFunc: makeDeal()" << std::endl;

    const char* PG_make_deal = "INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status) VALUES ($1, $2, $3, $4, $5, $6)";
    PGresult* result = PQexecParams(conn, PG_make_deal, 6, NULL, user_info, NULL, NULL, 0);

    PQclear(result);
    send_response("{\"success\"}");
}

void MySession::cancelDeal(PGconn* conn, const char* deal_info[]) {
    std::cout << "LogFunc: cancelDeal()" << std::endl;

    const char* PG_cancel_deal = "UPDATE deals SET deal_status = $2, time_end = NOW() WHERE id = $1 AND from_user_id = $3";
    PGresult* result = PQexecParams(conn, PG_cancel_deal, 3, NULL, deal_info, NULL, NULL, 0);

    PQclear(result);
    send_response("{\"success\"}");
}

void MySession::logReceivedJson(std::string method) {
    std::cout << "LogFunc: logReceivedJson()" << std::endl;

    if(method == "GET") {
        std::cout << "Log: no json"<< std::endl;
    }
    else if (method == "POST") {
        std::string body = get_request_body();
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

std::string MySession::get_request_body() {
    boost::beast::multi_buffer& body = request_.body();
    std::stringstream ss;

    for (const auto& buffer : body.data()) {
        ss << boost::beast::make_printable(buffer);
    }

    return ss.str();
}