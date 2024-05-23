# Welcome to LILAC

![](https://img.shields.io/github/license/Sharp0802/lilac)
![](https://img.shields.io/github/repo-size/Sharp0802/lilac)
![](https://img.shields.io/github/commit-activity/m/Sharp0802/lilac)

> [!IMPORTANT]
> ***WORKING IN PROGRESS***

*LILAC* is a **L**LVM based **I**nteroperability **LA**yer **C**ompiler -
with a goal that producing bindings across languages.

Interoperability across languages always sucks developers...
(Especially, For managed languages such as Java, C#, Python)

For example, between C++ and C#:

- C++/CLI couldn't support modern C++ well
- In addition, C++/CLI doesn't support GNU/Linux
- P/Invoke couldn't support C++ well because of name mangling etc.

But, with power of LLVM, we hope that LILAC will generate bindings between C++ and C# ...or what else!
(Actually, For the languages not supporting LLVM, We need to implement frontend for each languages)

**Let's break down language barrier!**

## Supported Languages

| Language |      Backend       |      Frontend      |
|:--------:|:------------------:|:------------------:|
|   C++    | :grey_exclamation: |        :o:         |
|    C#    |   :interrobang:    | :grey_exclamation: |

- :o: : Implemented.
- :grey_exclamation: : Not implemented, but planned.
- :interrobang: : Work in Progress

## Docs

- [INFRASTRUCTURE SPEC](docs/INFRASTRUCTURE.md)
