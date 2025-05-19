VENV = .venv
VENV_PYTHON3 = $(VENV)/bin/python3

ADMIN  ?= test:testpass
PROGRAM ?= ./client

all: venv deps client

venv: $(VENV_PYTHON3)
$(VENV_PYTHON3):
	python3 -m venv "$(VENV)"

deps: venv
	$(VENV_PYTHON3) -m pip install -r requirements.txt

A ?= --debug --admin "$(ADMIN)"
run:
	$(VENV_PYTHON3) checker.py $(PROGRAM) $(A) .


CC       ?= gcc
CFLAGS   ?= -std=gnu11 -Wall -Wextra -g
CPPFLAGS ?= -I. -Iparson

CLIENT_SRC := client.c helpers.c requests.c buffer.c parson.c

$(PROGRAM): $(CLIENT_SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

client: $(PROGRAM)

.PHONY: client clean
clean:
	rm -rf $(VENV) $(PROGRAM) *.o

