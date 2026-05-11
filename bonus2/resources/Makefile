override POSSIBLE_TARGET_LANG := nl fi

ifeq ($(TARGET_LANG),)
$(error TARGET_LANG unset, please set it to one of [$(POSSIBLE_TARGET_LANG:%="%")])
endif

ifeq ($(findstring $(TARGET_LANG),$(POSSIBLE_TARGET_LANG)),)
$(error TARGET_LANG invalid, please set it to one of [$(POSSIBLE_TARGET_LANG:%="%")])
endif

payload.bin: payload-$(TARGET_LANG).o
	objcopy -O binary -j .rodata $< $@

payload-$(TARGET_LANG).o: payload-$(TARGET_LANG).s
	gcc -m32 -Wall -Wextra -c -o $@ $<

.PHONY: clean
clean:
	$(RM) payload-*.bin payload-nl.o
