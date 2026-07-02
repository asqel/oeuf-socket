SRC = $(wildcard src/*/*.c src/*.c)
OBJ = $(SRC:.c=.o)
OBJ := $(OBJ:src/%=%)
OBJ := $(addprefix obj/win/, $(OBJ))

NAME_A = liboeso.win.a
NAME_SO = liboeso.dll

CC = x86_64-w64-mingw32-gcc
LD = $(CC)

LD_FLAGS = --shared -lws2_32 
CFLAGS = -Wall -Wextra -I. -DWIN32_LEAN_AND_MEAN=1

all: $(NAME_A) $(NAME_SO)

$(NAME_A): $(OBJ)
	ar rcs $@ $^

$(NAME_SO): $(OBJ)
	$(LD) $^ -o $@ $(LD_FLAGS)

obj/win/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME_A) $(NAME_SO)

re: fclean all

.PHONY: re fclean clean all 
