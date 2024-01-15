#include "requests.hpp"

TEST_F(APITest, Test_get_my_info) {
  auto responsePair = getTokenAndId("GetInfo");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;

  std::string response = getRequest(baseUrl + "/getMyInfo", token, user_id);

  ASSERT_FALSE(response.find("Unsupported path for GET request or invalid Token") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"first_name\":\"vova\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"second_name\":\"vasiliev\"") != std::string::npos);
}

TEST_F(APITest, Test_get_my_info_with_wrong_id) {
  auto responsePair = getTokenAndId("GetInfo");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;

  std::string response = getRequest(baseUrl + "/getMyInfo", token, "100");

  ASSERT_TRUE(response.find("Unsupported path for GET request or invalid Token") != std::string::npos);
}

TEST_F(APITest, Test_get_my_info_with_wrong_token) {
  auto responsePair = getTokenAndId("GetInfo");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;

  std::string response = getRequest(baseUrl + "/getMyInfo", "token", user_id);

  ASSERT_TRUE(response.find("Unsupported path for GET request or invalid Token") != std::string::npos);
}

TEST_F(APITest, Test_get_my_info_with_wrong_token_and_id) {
  std::string response = getRequest(baseUrl + "/getMyInfo", "token", "100");

  ASSERT_TRUE(response.find("Unsupported path for GET request or invalid Token") != std::string::npos);
}