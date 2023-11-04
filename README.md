## fiber-blog-example - Example code for a blog post I'm writing 🧶

This is a _super_ simple example of a low-level implementation of
cooperative user-mode fibers (virtual threads) in x86_64 assembly
and C intended as an illustration that this stuff doesn't have to
be all that complicated.

It was written to support a series of blog posts on the subject,
and is intended to be easy to follow - it is **not** optimized for
performance, does not attempt to cover all the edge cases, and
eschews error handling in favour of clarity.

That said, it _does_ work, and it should be fairly easy to add
those things in if you wanted to 🙂

### Building

> **Note**: Although this is written in x86_64 assembly for broad 
applicability and ease of understanding, it _does_ work on M1 
(or 2, or 3 I guess) thanks to Rosetta. You _will_ however need 
to install Nasm - it's available via `brew` but you _might_ need
to install an x86 version of brew alongside your regular arm64 one.

You will need a recent(ish) platform compiler. Tested with Clang
on macOS and GCC on Linux.

```shell
make clean all
```

This should build `main`, which you can run with `./main`.

### License

Copyright (c) 2023 Ross Bamford
See LICENSE.md (spoiler: MIT)

