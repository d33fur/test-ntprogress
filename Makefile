.PHONY: all
all: 
	clear-server
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

.PHONY: clear-server
clean-server:
	@docker-compose down || echo no containers to remove

# Клиентская часть

.PHONY: build-client
build-client:
	mkdir -p build && \
	cd build && \
	cmake .. && \
	make

.PHONY: clear-client
clean-client:
	rm -rf build