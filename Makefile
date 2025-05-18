# --------------------------- original ---------------------------
# Python + Flask makefile
VENV = .venv
VENV_PYTHON3 = $(VENV)/bin/python3

ADMIN  ?= test:testpass
PROGRAM ?= ./client

all: venv deps client          # adăugăm client aici, deci NU mai declarăm all::

venv: $(VENV_PYTHON3)
$(VENV_PYTHON3):
	python3 -m venv "$(VENV)"

deps: venv                     # rămâne single-colon, NU mai apare deps::
	$(VENV_PYTHON3) -m pip install -r requirements.txt

A ?= --debug --admin "$(ADMIN)"
run:
	$(VENV_PYTHON3) checker.py $(PROGRAM) $(A) .

# --------------------- ADĂUGAT pentru client --------------------
CC       ?= gcc
CFLAGS   ?= -std=gnu11 -Wall -Wextra -g
CPPFLAGS ?= -I. -Iparson          # pt. "parson/parson.h"

CLIENT_SRC := client.c helpers.c requests.c buffer.c parson/parson.c

# compilează binarul dorit de checker
$(PROGRAM): $(CLIENT_SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

# target intermediar comod, chemat de „all”
client: $(PROGRAM)

.PHONY: client clean
clean:
	rm -rf $(VENV) $(PROGRAM) *.o

