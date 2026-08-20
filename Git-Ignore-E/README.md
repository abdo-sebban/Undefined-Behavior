<p align="center"><img src="https://img.libquotes.com/pic-quotes/v2/ken-thompson-quote-lbv5x6k.jpg"></p>

You're reading some old C code, minding your own business, when you hit this:

```c
creat("file.txt", 0644);
```

You blink. You reread it. Somewhere, your editor's spellchecker quietly gives up.

`creat`. Not `create`. `creat`. Like someone was filling out a form, ran out of space in the box, and just... stopped.

This is not a typo. It's not some encoding artifact you can blame on old terminals. It's a real, standardized, POSIX-blessed function name that has been sitting in Unix, unbothered, since 1971.

And the man who wrote it has spent decades publicly regretting it.

<p align="center">
  <strong style="font-size: 1.8em;">"I'd spell creat with an e."</strong>
  <br>
  <sub>- Ken Thompson, on the one thing he'd change about Unix</sub>
</p>

<p align="center">
  <img src="https://commons.wikimedia.org/wiki/Special:FilePath/Ken_Thompson_and_Dennis_Ritchie--1973.jpg" alt="Ken Thompson and Dennis Ritchie, creators of Unix, 1973" width="420">
</p>
<p align="center"><sub>Ken Thompson (left) and Dennis Ritchie (right), 1973 - Wikimedia Commons</sub></p>

---

## So what does it actually do

`creat()` makes a new file, or truncates an existing one down to nothing, and hands you back a file descriptor open for writing. That's the entire job description.

Here's the part that makes it interesting: `creat()` isn't really its own function anymore, conceptually. POSIX defines its behavior as being *exactly* equivalent to one specific call to `open()`:

```c
int creat(const char *path, mode_t mode)
{
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}
```

That's the literal reference definition in the POSIX spec. Not "similar to." Not "roughly the same as." *Is.*

So everything `creat()` can do, `open()` already does, using the exact three flags `creat()` is built from. It's a function whose entire job has been fully absorbed by another function, and it's still standing there. In the standard. In glibc. In pretty much every Unix-like system you'll ever touch.

Keeping a fully redundant function around for over fifty years isn't an accident. It's a decision. We'll get to why in a minute.

---

## Back to the beginning

`creat()` wasn't bolted on later, it was there almost from day one. It shows up in the very first Unix Programmer's Manual, dated November 3, 1971, written by Ken Thompson and Dennis Ritchie at Bell Labs. Back then, `open()` didn't have the flexibility to create files the way it does now, so if you wanted a new file, `creat()` was *the* way to get one.

And the odd spelling was already there. This isn't something that drifted in later through porting or compatibility shims, it shipped that way from the very first manual, in the very first kernel.

There's a popular theory that this is a leftover from early filesystem or symbol-table character limits, that Unix ran on tight hardware, and short identifiers were common practice back then. It's a tidy story. It's just not a confirmed one. Plenty of other Unix system calls from the same era used perfectly normal-length names, and `creat` at five letters isn't meaningfully shorter than `create` at six. If there's a hard technical reason a specific letter had to go, nobody has ever produced documentation proving it.

So the honest answer is: nobody actually knows for certain why it's `creat` instead of `create`. It might just be how it got named, with no grand justification behind it at all. That uncertainty is basically the whole appeal of this story.

---

## The quote that made it canon

What we *do* have on solid record is Ken Thompson's own opinion on the matter, offered up decades later.

In *The UNIX Programming Environment* by Brian Kernighan and Rob Pike (Prentice-Hall, 1984), there's a footnote on page 204:

> Ken Thompson was once asked what he would do differently if he were redesigning the UNIX system. His reply: "I'd spell creat with an e."

That's the whole quote. Out of an entire operating system he co-invented, one that quietly reshaped how computers work, when asked what he'd change, his answer was about a single missing vowel.

It's since become one of the most-quoted lines in Unix folklore, repeated in books, mailing lists, and Stack Overflow threads for forty-plus years. Partly because it's funny. Partly because it says something true about software: sometimes the smallest, most throwaway decisions are the ones that outlive everything else you built.

---

## Why nobody just... fixed it

If `open()` already does everything `creat()` does, and even Ken Thompson wished it were spelled differently, why does `creat()` still exist, spelled exactly the same cursed way, today?

Backward compatibility. Boring answer. Extremely necessary in practice.

`creat()` has been called by an enormous number of programs since 1971. Rename it, remove it, "fix" it, and you break every single one of them at compile time. Standards bodies don't get to delete a function just because it's redundant; that's how "upgrade your OS" turns into "rewrite your codebase." POSIX is refreshingly blunt about this in its own rationale for `creat()`:

> The creat() function is redundant. Its services are also provided by the open() function. It has been included primarily for historical purposes since many existing applications depend on it.

That's not corporate hedging, that's the standard openly admitting *"yeah, we know, but ripping this out would hurt more than keeping it."* Nobody breaks fifty years of existing software over a spelling preference.

So `creat()` stays. Not because anyone thinks it's a good API surface, but because "it works and people depend on it" beats "it's a little embarrassing" every single time.

---

## The twist: Ken Thompson gets his `e` back

This is where it stops being a story about legacy cruft and turns into something genuinely satisfying.

Fast forward to 2009. Ken Thompson is at Google now, working on a brand-new language: Go. Naturally, Go's early `os` package needed a flag for "create this file on open," and, following Unix convention, it started out as `O_CREAT`.

Someone remembered the old quote. Go engineer Michael Jones has said he went to Ken directly, reminded him of what he'd said back in that 1984 Kernighan and Pike footnote, and asked whether he'd consider changing it. A few days later, Ken checked in the change himself.

The commit is real, and it's wonderful. `c90d392`, authored by Ken Thompson, November 10, 2009:

```diff
- O_CREAT
+ O_CREATE
```

> spell it with an "e"

One line. One flag. One letter, thirty-eight years later, added back by the exact same person who never fully explained why it went missing in the first place.

Unix itself never got its `e` back, `creat()` is still `creat()` today, unchanged, still redundant, still standardized, still sitting in header files everywhere. But in the one system Ken Thompson got to build without fifty years of existing software leaning on the old spelling, he quietly fixed it.

---

## tl;dr

- `creat()` appears in Unix's very first manual (1971), spelled that way from the start, for reasons nobody has ever definitively documented.
- It's fully redundant with `open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)`, and POSIX says so, in writing, in its own rationale.
- It survives purely because of backward compatibility: standards don't delete functions that existing software depends on.
- Ken Thompson himself said the one thing he'd change about Unix was spelling `creat` with an `e`.
- In 2009, building Go, he quietly went back and did exactly that.

<p align="center">
  <!-- Add image here: the actual Go commit / diff screenshot works great -->
</p>

---

## Sources / References

- [POSIX `creat()` specification and rationale](https://man7.org/linux/man-pages/man3/creat.3p.html) - IEEE Std 1003.1-2017 / The Open Group Base Specifications
- [First Edition Unix Programmer's Manual (1971)](https://s3.amazonaws.com/plan9-bell-labs/who/dmr/1stEdman.html) - original documentation, Ken Thompson & Dennis Ritchie
- Kernighan, Brian W.; Pike, Rob. *The UNIX Programming Environment*. Prentice-Hall, 1984. ISBN 0139376992, p. 204 - source of the "I'd spell creat with an e" quote
- [golang-nuts: "Curious about O_CREATE vs O_CREAT"](https://groups.google.com/g/golang-nuts/c/Mo1B9HxTCo4) - Michael Jones's account of asking Ken Thompson to make the change
- [Go commit `c90d392` - "spell it with an 'e'"](https://github.com/golang/go/commit/c90d392ce3d3203e0c32b3f98d1e68c4c2b4c49b)
- [Russ Cox, "Go's Version Control History"](https://research.swtch.com/govcs) - context on the commit and Go's early repository history
- [Ken Thompson and Dennis Ritchie, 1973 - Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Ken_Thompson_and_Dennis_Ritchie--1973.jpg)

---

<p align="center"><sub>Have a cursed, weird, or historically strange programming fact? PRs welcome.</sub></p>