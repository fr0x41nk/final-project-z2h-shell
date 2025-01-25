TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
CFLAGS = -g -Iinclude
#-fsanitize=address 
#LDFLAGS = -fsanitize=address

run: clean default
	rm -f mynewdb.db
	./$(TARGET) -n -f mynewdb.db
	./$(TARGET) -f mynewdb.db -a "Frank Solli,Jebeveien,1337"
	./$(TARGET) -f mynewdb.db -a "qbash,sandvika,1"
	./$(TARGET) -f mynewdb.db -a "ja,sandvika,2"
	./$(TARGET) -f mynewdb.db -a "a,k,5"
	./$(TARGET) -f mynewdb.db -a "linus,torvalds,1"
	./$(TARGET) -f mynewdb.db -l
	./$(TARGET) -f mynewdb.db -u 1,133
#	./$(TARGET) -f mynewdb.db -r 4
#	echo "removing\n\n\n\n\n"
#	./$(TARGET) -f mynewdb.db -r 1
	echo "listing again\n\n\n\n\n"
	./$(TARGET) -f mynewdb.db -l

# ./$(TARGET) -f mynewdb.db -
#	./$(TARGET) -f ./mynewdb.db -f mynewdb.db 
#	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"
#	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc $(LDFLAGS) -o $@ $?

obj/%.o : src/%.c
	gcc $(CFLAGS) -c $< -o $@
