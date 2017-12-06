all: control.c
	gcc -g -o control control.c

clean:
	rm control
	rm *.o

run: all
	./control -c 5
	./control -v
	./control -c 45
	./control -v
	./control -r
