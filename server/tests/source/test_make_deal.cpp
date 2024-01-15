#include "requests.hpp"

TEST_F(APITest, Test_make_deal_buy) {
  deleteDeals();
  deleteUsers();
  auto responsePair = getTokenAndId("MakeDeal");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "1"},
    {"exchange_rate", "60"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token, user_id);

  ASSERT_TRUE(response.find("success") != std::string::npos);
}

TEST_F(APITest, Test_make_deal_sell) {
  auto responsePair = getTokenAndId("MakeDeal");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "2"},
    {"exchange_rate", "61"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token, user_id);

  ASSERT_TRUE(response.find("success") != std::string::npos);
}

TEST_F(APITest, Test_make_deal_check_with_my_deals) {
  auto responsePair = getTokenAndId("MakeDeal");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;
  
  std::string response = getRequest(baseUrl + "/getMyDeals", token, user_id);
  json responseJson = json::parse(response);
  ASSERT_TRUE(response.find("\"deal_type\":\"buy\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"deal_type\":\"sell\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"exchange_rate\":\"60\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"exchange_rate\":\"61\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"deal_status\":\"active\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"amount\":\"1\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"amount\":\"2\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"from_user_id\":\"" + user_id + "\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"to_user_id\":\"0\"") != std::string::npos);
}