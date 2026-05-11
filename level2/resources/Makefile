payload.bin: payload.o
	objcopy -O binary -j .rodata $< $@

payload.o: payload.s
	gcc -Wall -Wextra -c -o $@ $<

.PHONY: clean
clean:
	$(RM) payload.bin payload.o
