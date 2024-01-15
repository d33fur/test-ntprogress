#include "requests.hpp"

TEST_F(APITest, Test_signup) {
  json bodyJson = {
    {"first_name", "vanya"},
    {"second_name", "vasiliev"},
    {"login", "ivan"},
    {"password", "crazypassword"},
    {"remember_me", "0"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/signup", body);

  ASSERT_TRUE(response.find("token") != std::string::npos);
  ASSERT_TRUE(response.find("user_id") != std::string::npos);
  ASSERT_FALSE(response.find("User already exists") != std::string::npos);
}

TEST_F(APITest, Test_signup_if_user_exists) {
  json bodyJson = {
    {"first_name", "vanya"},
    {"second_name", "vasiliev"},
    {"login", "ivan"},
    {"password", "crazypassword"},
    {"remember_me", "0"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/signup", body);

  ASSERT_FALSE(response.find("token") != std::string::npos);
  ASSERT_FALSE(response.find("user_id") != std::string::npos);
  ASSERT_TRUE(response.find("User already exists") != std::string::npos);
}
