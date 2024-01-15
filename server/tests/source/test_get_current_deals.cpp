#include "requests.hpp"

TEST_F(APITest, Test_get_current_deals) {
  deleteDeals();
  deleteUsers();
  auto responsePair = getTokenAndId("GetCDeals");
  std::string token = responsePair.first;
  std::string user_id = responsePair.second;
  auto responsePair1 = getTokenAndId("GetCDeals1");
  std::string token1 = responsePair1.first;
  std::string user_id1 = responsePair1.second;

    json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "3"},
    {"exchange_rate", "65"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token, user_id);

  response = getRequest(baseUrl + "/getCurrentDeals", token1, user_id1);
  ASSERT_TRUE(response.find("\"deal_type\":\"sell\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"exchange_rate\":\"65\"") != std::string::npos);;
  ASSERT_TRUE(response.find("\"amount\":\"3\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"from_user_id\":\"" + user_id + "\"") != std::string::npos);
}