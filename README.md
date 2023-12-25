# Тестовое задание в NTProgress
- [1. Постановка задачи](#1-Постановка-задачи)
    - [1.1. Что необходимо сделать](#11-Что-необходимо-сделать)
    - [1.2. Требования по реализации](#12-Требования-по-реализации)
    - [1.3. Технические требования](#13-Технические-требования)
- [2. Серверная часть](#2-Серверная-часть)
- [3. Клиентская часть](#3-Клиентская-часть)
- [4. Запуск и тесты](#4-Запуск-и-тесты)
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


# 2. Серверная часть

# 3. Клиентская часть

# 4. Запуск и тесты
Склонируйте репозиторий и перейдите в него

```
git clone https://github.com/d33fur/test-ntprogress.git && cd test-ntprogress
```
