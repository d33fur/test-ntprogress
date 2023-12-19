.PHONY: all
all: 
	clean-server
	start-server 
	clean-client
	build-client

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
restart-server:
	stop-server
	start-server

.PHONY: clean-server
clean-server:
	@docker-compose down || echo no containers to remove

# Клиентская часть
BUILD_DIR := cmake-build-client

.PHONY: build-client
build-client:
	mkdir -p $(BUILD_DIR) && \
	cd $(BUILD_DIR) && \
	cmake .. && \
	make

.PHONY: clean-client
clean-client:
	rm -rf $(BUILD_DIR)