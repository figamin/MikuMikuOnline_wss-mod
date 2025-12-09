# MikuMikuOnline 39chan Fork

This is a fork of the wss-mod MikuMikuOnline from 2021 for [MikuMikuTogether](https://mikumikutogether.neocities.org/), the main MikuMikuOnline English server. Here are some of the goals with this project:
- Translate all UI text into English (and potentially other languages)
- Greater stability on modern Windows
- Eliminate the need for Japanese Locale

Down the line we hope to make the client more dynamic, including customizable color and text settings as well as server news embedded in the client. We are also looking into making Linux native binaries.

This repository contains all the dependencies you need to build both client and server for x64 (Win32 is missing a few but that is lower priority). I have gotten this to compile successfully with Visual Studio 2010. It should be possible to compile this with newer versions of Visual Studio, just with recompiling some of the dependencies.

- The **master** branch will contain the latest source for the 39chan fork.
- The **original** branch will contain the unmodified code from the wss branch
- The **deprecated-master** branch is the renaming of the old master branch, which as far as I'm aware hasn't been used since before the wss fork.
