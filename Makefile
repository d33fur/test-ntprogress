.PHONY: all
all: rebuild-server start-server clear-client build-client start-client

.PHONY: clear-all
clear-all: clear-server clear-client
# Серверная часть

.PHONY: build-server
build-server:
	docker-compose build

.PHONY: rebuild-server
rebuild-server:
	docker-compose build --force-rm

.PHONY: start-server
start-server:
	docker-compose up -d

.PHONY: logs-server
logs-server:
	docker-compose logs -f --tail 100

.PHONY: stop-server
stop-server:
	docker-compose stop

.PHONY: restart-server
restart-server: stop-server start-server

.PHONY: clear-server
clear-server:
	@docker-compose down || echo no containers to remove

.PHONY: backend-all
backend-all: build-backend start-backend

.PHONY: build-backend
build-backend:
	cd server && \
	cd stock_exchange_service && \
	mkdir -p build && \
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: start-backend
start-backend:
	bash -c "while true; do ./server/stock_exchange_service/build/stock_exchange_service --port 8001; sleep 0; done"

# Клиентская часть

.PHONY: build-client
build-client:
	cd client && \
	mkdir -p build && \
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: start-client
start-client:
	./client/build/stock_exchange_client


.PHONY: clear-client
clear-client:
	rm -rf client/build