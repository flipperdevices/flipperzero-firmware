trezor-crypto tools
===================

Set of small utilities using the trezor-crypto library.

xpubaddrgen
-----------

xpubaddrgen reads job specification from stdin in format:

```
<jobid> <xpub> <change> <from> <to>
```

and prints the results to stdout in format:

```
<jobid> <index> <address>
```

Example input:

```
23 xpub6BcjTvRCYD4VvFQ8whztSXhbNyhS56eTd5P3g9Zvd3zPEeUeL5CUqBYX8NSd1b6Thitr8bZcSnesmXZH7KerMcc4tUkenBShYCtQ1L8ebVe 0 0 5
42 xpub6AT2YrLinU4Be5UWUxMaUz3zTA99CSGvXt1jt2Lgym8PqXbTzmpQ8MHjoLnx8YJiMMUP5iEfR97YQVmgF6B2tAhbCZrXqn65ur526NkZ6ey 1 1000 1005
```

Example output:

```
23 0 14vb5Cws75p2i5rmSiF5CKMyezUX4hxSb9
23 1 1Lf4ciA36dsi1niF6smVcpCiHcpj2skaPq
23 2 1LraByp7gQAipvHnFS1gTSzixBtYaVyQGp
23 3 1Hy6n56qZj1EefLVfDAeEpmveNteY9jpiG
23 4 183Nn4mrUjPizM3xu8C6SrmViaWrk8YyRS
42 1000 12eAFGAqGUtszc9R7euRqk7DUcQNXvQZSg
42 1001 1BrLbFCD3MNYedJaz92U9iqy9ukHrtQ1A6
42 1002 1Jhv33bJy229ThM7HKxUa92cMK5gi7DyPC
42 1003 13LxbTjQPByisj4F4sZEivUBdnJwigzg6R
42 1004 1BWBpSWkPwcKxVr2WDyUqQbmvk5SGihcx9
```

It will print ```<jobid> error``` when there was an error processing job jobid.

It will print ```error``` when it encountered a malformed line.


mktable
-----------

mktable computes the points of the form `(2*j+1)*16^i*G` and prints them in the format to be included in `secp256k1.c` and `nist256p1.c`.
These points are used by the fast ECC multiplication.

It is only meant to be run if the `scalar_mult` algorithm changes.
