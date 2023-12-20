CC = cc
CFLAGS = -Wall -Wextra -pedantic

DEFAULT: ccwc step_1 step_2 step_3 step_4 step_5 final_step

ccwc: ccwc.c
	$(CC) $(CFLAGS) $^ -o $@

step_1: ccwc
	@echo "Step 1:"
	wc -c test.txt
	./ccwc -c test.txt

step_2: ccwc
	@echo "Step 2:"
	wc -l test.txt
	./ccwc -l test.txt

step_3: ccwc
	@echo "Step 3:"
	wc -w test.txt
	./ccwc -w test.txt

step_4: ccwc
	@echo "Step 4:"
	wc -m test.txt
	./ccwc -m test.txt

step_5: ccwc
	@echo "Step 5:"
	wc test.txt
	./ccwc test.txt

final_step: ccwc
	@echo "Final step:"
	cat test.txt | wc -l
	cat test.txt | ./ccwc -l
