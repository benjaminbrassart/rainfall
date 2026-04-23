Looking at the source representation, we can immediately see some issues:
* in the `auth` command, `strcpy(3)` is used on a heap buffer of 4 bytes and is guarded by a bounds check of 30 bytes
* in the `reset` command, the `auth` buffer is `free`d but never reset to `NULL`
* the win condition is guarded by a check at index 32 of the `auth` buffer which holds 4 bytes

As we saw in previous levels, `malloc(3)` allocates chunks next to each other.  We can confirm this:

```
(nil), (nil)
auth 1
0x804a008, (nil)
service 2
0x804a008, 0x804a018
```

The `service` buffer is 16 bytes after the `auth` buffer.  The win condition is `auth[32] != '\0'`, and `auth[32]` is basically `*(0x804a008 + 0x20)` or `*(0x804a028)`.  We can reach and write at this address by using the `service` buffer and the `service` command.  We just need to allocate the `auth` buffer and allocate a `service` buffer longer than 16 bytes.  To allocate the later, we need to type at least 27 characters since it calls `strdup(3)` at the 11th character of the read buffer (16 + 11 = 27).  Less characters will also work because of the metadata of the next chunk 

```
(nil), (nil)
auth 1
0x804a008, (nil)
service qqq0123456789abcdef
0x804a008, 0x804a018
login
```
