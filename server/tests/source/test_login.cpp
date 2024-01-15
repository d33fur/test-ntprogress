#include "requests.hpp"

TEST_F(APITest, Test_login) {
  auto responsePair = getTokenAndId("vova");

  json bodyJson = {
    {"login", "vova"},
    {"password", "crazypassword"},
    {"remember_me", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/login", body);

  ASSERT_TRUE(response.find("token") != std::string::npos);
  ASSERT_TRUE(response.find("user_id") != std::string::npos);
  ASSERT_FALSE(response.find("Wrong login or password") != std::string::npos);
}

TEST_F(APITest, Test_login_with_wrong_password) {
  auto responsePair = getTokenAndId("vova");

  json bodyJson = {
    {"login", "vova"},
    {"password", "crazy"},
    {"remember_me", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/login", body);
  
  ASSERT_FALSE(response.find("token") != std::string::npos);
  ASSERT_FALSE(response.find("user_id") != std::string::npos);
  ASSERT_TRUE(response.find("Wrong login or password") != std::string::npos);
}

TEST_F(APITest, Test_login_with_wrong_login) {
  auto responsePair = getTokenAndId("vova");

  json bodyJson = {
    {"login", "vova_vova"},
    {"password", "crazypassword"},
    {"remember_me", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/login", body);
  
  ASSERT_FALSE(response.find("token") != std::string::npos);
  ASSERT_FALSE(response.find("user_id") != std::string::npos);
  ASSERT_TRUE(response.find("Wrong login or password") != std::string::npos);
}

TEST_F(APITest, Test_login_with_wrong_login_and_password) {
  auto responsePair = getTokenAndId("vova");

  json bodyJson = {
    {"login", "vova123"},
    {"password", "crazy"},
    {"remember_me", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequest(baseUrl + "/login", body);

  ASSERT_FALSE(response.find("token") != std::string::npos);
  ASSERT_FALSE(response.find("user_id") != std::string::npos);
  ASSERT_TRUE(response.find("Wrong login or password") != std::string::npos);
}