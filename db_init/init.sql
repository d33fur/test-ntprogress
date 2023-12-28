CREATE DATABASE stock_exchange

\c stock_exchange

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    first_name VARCHAR(255) NOT NULL,
    second_name VARCHAR(255) NOT NULL,
    balance_USD INTEGER,
    balance_RUB INTEGER,
    login VARCHAR(255) NOT NULL UNIQUE,
    password VARCHAR(255) NOT NULL
);
CREATE TYPE valid_deal_type AS ENUM ('buy', 'sell');
CREATE TABLE deals (
    id SERIAL PRIMARY KEY,
    from_user_id INTEGER NOT NULL,
    to_user_id INTEGER,
    deal_type valid_deal_type,
    amount INTEGER NOT NULL,
    exchange_rate INTEGER NOT NULL,
    deal_status VARCHAR(255) NOT NULL,
    time_start TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    time_end TIMESTAMP,
);

INSERT INTO users (first_name, second_name, balance_USD, balance_RUB, login, password)
VALUES 
    ('John', 'Doe', 1000, 50000, 'john_doe', 'password123'),
    ('Alice', 'Smith', 500, 25000, 'alice_smith', 'securepass');

INSERT INTO deals (from_user_id, to_user_id, deal_type, amount, exchange_rate, deal_status)
VALUES
    (1, 2, 'buy', 10, 75, 'completed');