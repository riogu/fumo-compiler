so the plan right now is:

- not work on the frontend until we have worked on a proper working backend with llvm ir
> right now i have done good progress on the parser, and most important features are being parser
> so there is no need to work further on the parser.

- work on the llvm ir backend
> the main idea is to make a working backend with llvm before the semantic checking.
> i will simply only test the IR with working conforming programs, and get that to work.
> then later i will add proper semantic (and type) checking to the language.

- steps:
  - make a small test of llvm features in a single .cpp file
  - slowly add llvm to my compiler starting from arithmetic and a simple translation unit
