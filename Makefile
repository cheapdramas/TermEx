APP_NAME = termex

SRC = explorer.c

INSTALL_DIR = /usr/bin

CC = gcc
CFLAGS = -lncurses

.PHONY: all install uninstall clean

all: $(APP_NAME)

$(APP_NAME): $(SRC)
	@echo "Компилируем $(APP_NAME)..."
	$(CC) $(SRC) $(CFLAGS) -o $(APP_NAME)


install: $(APP_NAME)
	sudo apt-get update
	sudo apt-get install -y libncurses5-dev libncursesw5-dev
	sudo cp $(APP_NAME) $(INSTALL_DIR)

uninstall:
	sudo rm -f $(INSTALL_DIR)/$(APP_NAME)

clean:
	rm -f $(APP_NAME)
