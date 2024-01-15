# Тестовое задание в NTProgress
- [1. Постановка задачи](#1-Постановка-задачи)
    - [1.1. Что необходимо сделать](#11-Что-необходимо-сделать)
    - [1.2. Требования по реализации](#12-Требования-по-реализации)
    - [1.3. Технические требования](#13-Технические-требования)
- [2. Реализация](#2-Реализация)
    - [2.1. Сервер](#21-Сервер)
        - [2.1.1. Базы данных](#211-Базы-данных)
        - [2.1.2. api](#212-api)
    - [2.2. Клиент](#21-Клиент)
- [3. Запуск и тесты](#3-Запуск-и-тесты)
# 1. Постановка задачи
## 1.1 Что необходимо сделать
Необходимо написать простую биржу с клиент-серверной архитектурой. Для упрощения задачи, биржа будет торговать только долларами (USD) за рубли (RUB).

Сервер должен принимать заявки на покупку или продажу долларов за рубли от разных клиентов по определённой цене.

В случае, если цена на покупку и цена на продажу у нескольких клиентов пересекается — нужно заключать сделки между ними. В этом случае купленный объём будет зачисляться на баланс клиентам.
### ℹ️ Условия:
#### Сервер

- Поддерживает подключения нескольких клиентов одновременно.
- Даёт возможность просмотра клиентом всех своих активных торговых заявок, всех своих совершенных сделок и своего баланса.
#### Клиент

- Подключается к серверу и реализует все его возможности.
#### Торговая логика

- Торговая заявка содержит объём, цену и сторону (покупка/продажа).
- Торговая заявка активна до тех пор, пока по ней есть активный объём.
- Если две заявки пересекаются по цене — для сделки выбирается цена более ранней заявки.
- Если заявка пересекается с несколькими заявками по другой стороне — приоритет в исполнении отдаётся заявкам с максимальной ценой для заявок на покупку и с минимальной ценой для заявок на продажу.
- Возможно частичное исполнение заявки. (см. пример)
- Баланс клиента не ограничен — он может торговать в минус.
#### 📝 Пример:

- Пользователь 1 выставил заявку на покупку 10 USD за RUB по цене 62.
- Пользователь 2 выставил заявку на покупку 20 USD за RUB по цене 63.
- Пользователь 3 выставил заявку на продажу 50 USD за RUB по цене 61.
- Сервер сматчил эти заявки между собой. Получилась две сделки:
  - На 20$ по цене 63 между П2 и П3.
  - На 10$ по цене 62 между П1 и П3.
- Теперь на балансе пользователей:
  - Пользователь 1: 10 USD, -620 RUB.
  - Пользователь 2: 20 USD, -1260 RUB.
  - Пользователь 3: -30 USD, 1880 RUB.
- Торговая заявка пользователя 3 (частично исполненная на 30$) осталась активной на 20$.
| ⚠️ Важно! Мы исполняемся в таком порядке неслучайно: начали с заявки П2, поскольку её цена больше, чем у П1. 

#### 🎈 Бонусы! Опционально реализуйте: 

- Возможность просмотра клиентом котировок 📈
- Получение отчёта о совершённой сделке в момент её заключения всеми сторонами-участниками. 🤼
- Возможность отмены активных заявок клиентами. 🚫
- Сохранение истории всех заявок и сделок в базу данных (например, PostgreSQL). 💽
- Авторизацию клиента с паролем. 🔑
- GUI к бирже на QT. 🤯
## 1.2 Требования по реализации
- Клиент может представлять собой обычное консольное приложение.
- Все основные функции должны быть покрыты тестами.

## 1.3 Технические требования
- Проект должен быть написан с использованием C++17/20.
- Разрешается использование сторонних библиотек.
- Проект должен собираться с использованием CMake.
- Описание, инструкция по сборке и инструкция по использованию проекта должны быть в файле README.md
- Проект должен быть выполнен в git-репозитории, ссылку на который необходимо предоставить.


# 2. Реализация
Реализованные Бонусы

|бонус|реализовано?|
|----|----|
|Возможность просмотра клиентом котировок 📈|на сервере - да, на клиенте - нет|
|Получение отчёта о совершённой сделке в момент её заключения всеми сторонами-участниками. 🤼|нет|
|Возможность отмены активных заявок клиентами. 🚫|да|
|Сохранение истории всех заявок и сделок в базу данных (например, PostgreSQL). 💽|да|
|Авторизацию клиента с паролем. 🔑|да|
|GUI к бирже на QT. 🤯|да|

## 2.1 Сервер
Реализовано на Boost.

Сборка через cmake и запуск в докере, реализовано хранение данных в PostgreSQL, хранение токенов сессий для каждого пользователя в Redis. 

При авторизации отправляется токен сессии для пользователя и последующие запросы на сервер происходят по токену сессии.

### 2.1.1 Базы данных
#### PostgreSQL
таблицы:

|users|deals|
|-|-|
|---------------|---------------|
|id|id|
|first_name|from_user_id|
|second_name|to_user_id|
|balance_USD|deal_type|
|balance_RUB|amount|
|login|exchange_rate|
|password|deal_status|
||time_start|
||time_end|

#### Redis

|key|value|
|-|-|
|user:<user_id>:token|<token>|

### 2.1.2 api
### POST запросы
### ```/makeDeal```
Создает заявку.

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Body:
```json
{
    "deal_type": "buy",
    "amount": "100",
    "exchange_rate": "60"
}
```
Ничего не возвращает.

### ```/cancelDeal```
Отменяет заявку.

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Body:
```json
{
    "deal_id": "7"
}
```

Ничего не возвращает.

### ```/login```
Авторизация по логину и паролю, если успешно, то возвращает token и id пользователя.

Headers: none

Body:
```json
{
    "login": "1",
    "password": "1",
    "remember_me": "1"
}
```
(с remember_me была идея создавать временный токен, по которому после сессии нельзя было зайти)

Reply:
Если успешно
```json
{
    "token":"gVfIhbBitZ5N746SBzFy42yAlmUaYbslAKw9ttaLzNPGafkyBtTzwWA0vzAYr7CQ011HoRykXA7Fddk8ikXKdiG8YFVqEE5LyW2JJZlXTaM9WOMTccbfwlGbnDT8S7Md",
    "user_id":"24"
}
```
Если неуспешно
```json
{
    "Wrong login or password"
}
```

### ```/signup```
Регистрация по логину и паролю, если успешно, то возвращает token и id пользователя.

Headers: none

Body:
```json
{
    "first_name": "vanya",
    "second_name": "vasiliev",
    "login": "ivan2",
    "password": "crazypassword",
    "remember_me": "0"
}
```
(с remember_me была идея создавать временный токен, по которому после сессии нельзя было зайти)

Reply:
Если успешно
```json
{
    "token":"gVfIhbBitZ5N746SBzFy42yAlmUaYbslAKw9ttaLzNPGafkyBtTzwWA0vzAYr7CQ011HoRykXA7Fddk8ikXKdiG8YFVqEE5LyW2JJZlXTaM9WOMTccbfwlGbnDT8S7Md",
    "user_id":"24"
}
```
Если неуспешно
```json
{
    "User already exists"
}
```

### GET запросы

### ```/checkToken```
Вот как раз это для remember_me

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Reply:
Если успешно
```json
{
    "is_user_valid": "1"
}
```
Если неуспешно
```json
{
    "is_user_valid": "0"
}
```

### ```/getCurrentDeals```
Запрос на получение текущих активных заявок

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Reply:
```json
[
    {
        "amount": "100",
        "deal_status": "active",
        "deal_type": "buy",
        "exchange_rate": "60",
        "from_user_id": "1",
        "id": "2",
        "time_start": "2023-12-19 09:41:32.716872",
        "to_user_id": "2"
    },
    ...
    {
        "amount": "100",
        "deal_status": "active",
        "deal_type": "buy",
        "exchange_rate": "60",
        "from_user_id": "1",
        "id": "4",
        "time_start": "2023-12-19 09:45:08.023111",
        "to_user_id": "2"
    }
]
```

### ```/getMyDeals```
Запрос на получение заявок пользователя

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Reply:
```json
[
    {
        "amount": "100",
        "deal_status": "active",
        "deal_type": "buy",
        "exchange_rate": "60",
        "from_user_id": "19",
        "id": "6",
        "time_end": "",
        "time_start": "2023-12-23 02:49:30.994991",
        "to_user_id": "0"
    },
    ...
    {
        "amount": "100",
        "deal_status": "active",
        "deal_type": "buy",
        "exchange_rate": "60",
        "from_user_id": "19",
        "id": "8",
        "time_end": "",
        "time_start": "2023-12-23 02:49:52.492691",
        "to_user_id": "0"
    }
]
```

### ```/getMyInfo```
Запрос на получение информации о пользователе(имя, фамилия, баланс)

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Reply:
```json
{
    "balance_rub": "0",
    "balance_usd": "0",
    "first_name": "vanya",
    "second_name": "vasiliev"
}
```

### ```/getCurrencyQuotes```
Запрос на получение данных для построения котировок

Headers:
|key|value|
|-|-|
|Token|OppAqpglzvrC17UQWCYqscHrNs6DfVFMRSKCQWK5UKOxOKpd74Z86QUEml24wSiQ3i9Fh2CLnBcK0KHVLY3DTyj8s5ujEphpqfHGSU6pnljmGEXmp2uqLxwkOwqrZ6Xy|
|User_id|19|

Reply:
```json
[
    {
        "exchange_rate": "34",
        "time_start": "2023-12-26 17:12:44.614782"
    },
    {
        "exchange_rate": "223",
        "time_start": "2023-12-26 10:30:09.61555"
    },
    ...
    {
        "exchange_rate": "222",
        "time_start": "2023-12-26 10:28:39.898575"
    }
]
```
## 2.2 Клиент
Реализовано на Qt5.

Сборка через cmake.

"Remember me" не работает на клиенте, но реализация на бэкэнде есть, так же как и с котировками.

# 3. Запуск и тесты
Для работы всего нужно поставить
```bash
sudo apt install -y make docker docker-compose postgresql libboost-dev libboost-program-options-dev cmake build-essential libpq-dev libhiredis-dev qt5-base-dev 
```
Далее склонируйте репозиторий и перейдите в него
```bash
git clone https://github.com/d33fur/test-ntprogress.git && cd test-ntprogress
```
Запуск
```bash
make all
```
Для очистки и выключения контейнеров
```bash
make clear-all
```
Логи бэкэнда
```bash
make logs-server
```
Больше комманд можно увидеть внутри Makefile

~~P.S. в связи с зачетной неделей и предновогодними приготовлениями не успел исправить ошибку сегментации при отправке ответа с бэкэнда и написать тесты.~~

~~Из за ошибки сегментации бэк нужно запускать отдельно~~
```bash
make backend-all
```
P.P.S.
1) Починил ошибку сегментации, теперь можно запускать через ```make all```.
2) Благодаря тестам нашел несколько ошибок и исправил их.
3) Написал тесты, запуск записал в ```make all```, но можно и отдельно запускать(подробнее в Makefile).
4) Тесты на логику биржи можно было покомпактнее написать.
5) Для запуска ```make backend-all```, ```build-backend``` и ```start-backend``` необходимо поменять айпи баз данных в коде с их айпи в сети докера на локалку.
6) Структурировал проект. Клиент выглядит вроде неплохо, но в бэке 35+ функций в одном файле - эт плохо. Можно было декомпозировать, но я этого не сделал.