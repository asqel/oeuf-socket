SRC = $(wildcard src/*/*.c src/*.c)
OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:src/%=%)
OBJ := $(addprefix obj/linux/, $(OBJ))

NAME_A = liboeso.a
NAME_SO = liboeso.so

CC = gcc
LD = $(CC)

LD_FLAGS = --shared 
CFLAGS = -Wall -Wextra -I.

all: $(NAME_A) $(NAME_SO)

$(NAME_A): $(OBJ)
	ar rcs $@ $<

$(NAME_SO): $(OBJ)
	$(LD) $< -o $@ $(LD_FLAGS) 

obj/linux/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME_A) $(NAME_SO)

re: fclean all

.PHONY: re fclean clean all 
