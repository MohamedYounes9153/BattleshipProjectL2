CC = gcc
OPTIONS = -Wall -ansi -pedantic
EXECUTABLE = bataille_navale
# Ne pas modifier ci-dessous
SOURCE = $(wildcard *.c)
OBJETS = $(SOURCE:.c=.o)
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJETS)
	$(CC) $(OPTIONS) $(OBJETS) -o $(EXECUTABLE)
%.o: %.c
	$(CC) $(OPTIONS) -MMD -c $<
clean:
	rm -vf *.o *.d
-include *.d