build:
	@gcc main.c -o ./bin/main -lm
run: build
	@./bin/main
