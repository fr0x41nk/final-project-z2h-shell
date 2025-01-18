TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -n -f mynewdb.db
	./$(TARGET) -f mynewdb.db -a "Frank Solli,Jebeveien,1337" 
#	//Navn, Adresse, Timer jobbet
#	./$(TARGET) -f mynewdb.db
#	./$(TARGET) -f ./mynewdb.db -f mynewdb.db 
#	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"
#	./$(TARGET) -f ./mynewdb.db -a "Timmy H.,123 Sheshire Ln.,120"

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude


