TARGET_SRV = bin/dbserver
TARGET_CLI = bin/dbcli

SRC_SRV = $(wildcard src/srv/*.c)
OBJ_SRV = $(SRC_SRV:src/srv/%.c=obj/srv/%.o)

SRC_CLI = $(wildcard src/cli/*.c)
OBJ_CLI = $(SRC_CLI:src/cli/%.c=obj/cli/%.o)

run: clean default
	./$(TARGET_SRV) -f ./mynewdb.db -n -p 8080 &
	./$(TARGET_CLI) 127.0.0.1
	kill -9 $$(pidof dbserver)

default: $(TARGET_SRV) $(TARGET_CLI)

clean:
	rm -f obj/srv/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET_SRV): $(OBJ_SRV)
	gcc -o $@ $?

$(OBJ_SRV): obj/srv/%.o: src/srv/%.c
	gcc -c $< -o $@ -Iinclude

$(TARGET_CLI): $(OBJ_CLI)
	gcc -o $@ $?

$(OBJ_CLI): obj/cli/%.o: src/cli/%.c
	gcc -c $< -o $@ -Iinclude





# TARGET = bin/dbview




# SRC = $(wildcard src/*.c)
# OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
# CFLAGS = -g -Iinclude
# #-fsanitize=address 
# #LDFLAGS = -fsanitize=address

# run: clean default
# 	rm -f mynewdb.db
# 	./$(TARGET) -n -f mynewdb.db
# 	./$(TARGET) -f mynewdb.db -a "Frank Solli,Jebeveien,1337"
# 	./$(TARGET) -f mynewdb.db -a "qbash,sandvika,1"
# 	./$(TARGET) -f mynewdb.db -a "ja,sandvika,2"
# 	./$(TARGET) -f mynewdb.db -a "a,k,5"
# 	./$(TARGET) -f mynewdb.db -a "linus,torvalds,1"
# 	./$(TARGET) -f mynewdb.db -l
# 	./$(TARGET) -f mynewdb.db -u 1,133
# 	echo "listing again\n\n\n\n\n"
# 	./$(TARGET) -f mynewdb.db -l


# default: $(TARGET)

# clean:
# 	rm -f obj/*.o
# 	rm -f bin/*
# 	rm -f *.db

# $(TARGET): $(OBJ)
# 	gcc $(LDFLAGS) -o $@ $?

# obj/%.o : src/%.c
# 	gcc $(CFLAGS) -c $< -o $@
