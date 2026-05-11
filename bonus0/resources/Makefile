payload.bin: payload.o
	objcopy -O binary -j .rodata $< $@

payload.o: payload.s
	gcc -m32 -Wall -Wextra -c -o $@ $<

.PHONY: clean
clean:
	$(RM) payload.bin payload.o
