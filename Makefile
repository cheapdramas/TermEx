APP_NAME = termex

SRC = explorer.c

INSTALL_DIR = /usr/bin

CC = gcc
CFLAGS = $(shell pkg-config ncursesw --libs --cflags)

.PHONY: all install uninstall clean

all: $(APP_NAME)

$(APP_NAME): $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(APP_NAME)

install: $(APP_NAME)
	sudo apt-get update
	sudo apt-get install -y libncurses5-dev libncursesw5-dev pkg-config
	sudo apt-get install fzf
	sudo cp $(APP_NAME) $(INSTALL_DIR)

uninstall:
	sudo rm -f $(INSTALL_DIR)/$(APP_NAME)

clean:
	rm -f $(APP_NAME)

