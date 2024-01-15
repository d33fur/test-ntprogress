#include <gtest/gtest.h>
#include <string>
#include <cpr/cpr.h>
#include "json.hpp"
#include <libpq-fe.h>

using json = nlohmann::json;

class APITest : public ::testing::Test {
protected:
  std::string postRequest(const std::string& url, const std::string& body) {
    cpr::Response response = cpr::Post(cpr::Url{url}, cpr::Body{body}, cpr::Header{{"Content-Type", "application/json"}});
    return response.text;
  }

  std::string postRequestWithTokenAndUserId(const std::string& url, const std::string& body, const std::string& token, const std::string& user_id) {
    cpr::Header headers = {
      {"Content-Type", "application/json"},
      {"Token", token},
      {"User_id", user_id}
    };

    cpr::Response response = cpr::Post(cpr::Url{url}, cpr::Body{body}, headers);
    return response.text;
  }

  std::string getRequest(const std::string& url, const std::string& token, const std::string& user_id) {
      cpr::Header headers = {
      {"Content-Type", "application/json"},
      {"Token", token},
      {"User_id", user_id}
    };

    cpr::Response response = cpr::Get(cpr::Url{url}, headers);
    return response.text;
  }

  std::pair<std::string, std::string> getTokenAndId(const std::string& login) {
    json bodyJsonForLogin = {
      {"first_name", "vova"},
      {"second_name", "vasiliev"},
      {"login", login},
      {"password", "crazypassword"},
      {"remember_me", "0"}
    };
    std::string bodyForLogin = bodyJsonForLogin.dump();
    std::string responseForLogin = postRequest(baseUrl + "/signup", bodyForLogin);

    std::string token;
    std::string user_id;

    if(responseForLogin.find("User already exists") != std::string::npos) {
      bodyJsonForLogin = {
        {"login", login},
        {"password", "crazypassword"},
        {"remember_me", "0"}
      };
      responseForLogin = postRequest(baseUrl + "/login", bodyForLogin);
      auto responseJsonForLogin = json::parse(responseForLogin);
      token = responseJsonForLogin["token"];
      user_id = responseJsonForLogin["user_id"];
    }
    else {
      auto responseJsonForLogin = json::parse(responseForLogin);
      token = responseJsonForLogin["token"];
      user_id = responseJsonForLogin["user_id"];
    }
    return {token, user_id};
  }

  void deleteDeals() {
    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=0.0.0.0 port=5432";    
    PGconn* conn = PQconnectdb(conninfo);
    const char* PG_update_deal_info = "DELETE FROM deals";
    PGresult* result = PQexec(conn, PG_update_deal_info);
    PQclear(result);
  }

  void deleteUsers() {
    const char* conninfo = "dbname=stock_exchange user=admin123 password=password123 host=0.0.0.0 port=5432";    
    PGconn* conn = PQconnectdb(conninfo);
    const char* PG_update_deal_info = "DELETE FROM users";
    PGresult* result = PQexec(conn, PG_update_deal_info);
    PQclear(result);
  }

  std::string baseUrl = "http://0.0.0.0:8001";
};

