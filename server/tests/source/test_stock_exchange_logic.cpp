#include "requests.hpp"

TEST_F(APITest, Test_test_example) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "62"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "63"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "50"},
    {"exchange_rate", "61"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-620\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"10\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-1260\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"20\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"1880\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-30\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_1) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "62"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "63"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "50"},
    {"exchange_rate", "65"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  responsePair = getTokenAndId("SELogic4");
  std::string token4 = responsePair.first;
  std::string user_id4 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "5"},
    {"exchange_rate", "61"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token4, user_id4);

  responsePair = getTokenAndId("SELogic5");
  std::string token5 = responsePair.first;
  std::string user_id5 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "50"},
    {"exchange_rate", "65"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token5, user_id5);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-3250\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"50\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token4, user_id4);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token5, user_id5);

  ASSERT_TRUE(response.find("\"balance_rub\":\"3250\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-50\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_2) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "10"},
    {"exchange_rate", "50"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "100"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"1000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-10\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-1000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"10\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_3) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "50"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_4) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "50"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "100"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_5) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "50"}
  };
  body = bodyJson.dump(); 
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-1000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"10\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"1000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-10\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_6) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "50"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-2000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"20\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"2000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-20\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_7) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "50"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "100"},
    {"exchange_rate", "100"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "15"},
    {"exchange_rate", "75"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "75"},
    {"exchange_rate", "60"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  responsePair = getTokenAndId("SELogic4");
  std::string token4 = responsePair.first;
  std::string user_id4 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "15"},
    {"exchange_rate", "90"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token4, user_id4);

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "15"},
    {"exchange_rate", "95"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-11000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"110\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"2075\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-20\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"7500\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-75\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token4, user_id4);

  ASSERT_TRUE(response.find("\"balance_rub\":\"1425\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-15\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_8) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10"},
    {"exchange_rate", "100"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "50"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "100"},
    {"exchange_rate", "100"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "15"},
    {"exchange_rate", "75"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "75"},
    {"exchange_rate", "60"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  responsePair = getTokenAndId("SELogic4");
  std::string token4 = responsePair.first;
  std::string user_id4 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "15"},
    {"exchange_rate", "90"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token4, user_id4);

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "15"},
    {"exchange_rate", "95"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-11000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"110\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"2075\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-20\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"7500\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-75\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token4, user_id4);

  ASSERT_TRUE(response.find("\"balance_rub\":\"1425\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-15\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_9) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "65"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "20"},
    {"exchange_rate", "65"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_10) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "150"},
    {"exchange_rate", "63"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "20"},
    {"exchange_rate", "64"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "30"},
    {"exchange_rate", "63"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  responsePair = getTokenAndId("SELogic4");
  std::string token4 = responsePair.first;
  std::string user_id4 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "5"},
    {"exchange_rate", "64"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token4, user_id4);

  responsePair = getTokenAndId("SELogic5");
  std::string token5 = responsePair.first;
  std::string user_id5 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "75"},
    {"exchange_rate", "61"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token5, user_id5);

  responsePair = getTokenAndId("SELogic6");
  std::string token6 = responsePair.first;
  std::string user_id6 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "150"},
    {"exchange_rate", "70"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token6, user_id6);

  responsePair = getTokenAndId("SELogic7");
  std::string token7 = responsePair.first;
  std::string user_id7 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "175"},
    {"exchange_rate", "60"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token7, user_id7);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-4725\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"75\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-1280\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"20\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"0\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"0\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token4, user_id4);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-320\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"5\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token5, user_id5);

  ASSERT_TRUE(response.find("\"balance_rub\":\"4750\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-75\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token6, user_id6);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-10500\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"150\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token7, user_id7);

  ASSERT_TRUE(response.find("\"balance_rub\":\"12075\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-175\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_11) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "100"},
    {"exchange_rate", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "99"},
    {"exchange_rate", "75"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"7425\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-99\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-7425\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"99\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_12) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "100"},
    {"exchange_rate", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "100"},
    {"exchange_rate", "75"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "100"},
    {"exchange_rate", "60"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  responsePair = getTokenAndId("SELogic4");
  std::string token4 = responsePair.first;
  std::string user_id4 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "201"},
    {"exchange_rate", "75"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token4, user_id4);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"75\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-1\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"7500\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-100\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"7500\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-100\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token4, user_id4);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-15075\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"201\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_13) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "buy"},
    {"amount", "7"},
    {"exchange_rate", "60"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "sell"},
    {"amount", "99"},
    {"exchange_rate", "1"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-420\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"7\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"420\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-7\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_14) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "100"},
    {"exchange_rate", "1"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "99"},
    {"exchange_rate", "5"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  responsePair = getTokenAndId("SELogic3");
  std::string token3 = responsePair.first;
  std::string user_id3 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "50"},
    {"exchange_rate", "60"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token3, user_id3);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"555\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-100\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-495\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"99\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token3, user_id3);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-60\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"1\"") != std::string::npos);
}

TEST_F(APITest, Test_logic_15) {
  deleteDeals();
  deleteUsers();

  auto responsePair = getTokenAndId("SELogic1");
  std::string token1 = responsePair.first;
  std::string user_id1 = responsePair.second;

  json bodyJson = {
    {"deal_type", "sell"},
    {"amount", "10000"},
    {"exchange_rate", "10000"}
  };
  std::string body = bodyJson.dump();
  std::string response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token1, user_id1);

  responsePair = getTokenAndId("SELogic2");
  std::string token2 = responsePair.first;
  std::string user_id2 = responsePair.second;

  bodyJson = {
    {"deal_type", "buy"},
    {"amount", "10000"},
    {"exchange_rate", "10000"}
  };
  body = bodyJson.dump();
  response = postRequestWithTokenAndUserId(baseUrl + "/makeDeal", body, token2, user_id2);

  response = getRequest(baseUrl + "/getMyInfo", token1, user_id1);

  ASSERT_TRUE(response.find("\"balance_rub\":\"100000000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"-10000\"") != std::string::npos);

  response = getRequest(baseUrl + "/getMyInfo", token2, user_id2);

  ASSERT_TRUE(response.find("\"balance_rub\":\"-100000000\"") != std::string::npos);
  ASSERT_TRUE(response.find("\"balance_usd\":\"10000\"") != std::string::npos);
}