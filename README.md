# What is this ?
This code compares the performances of AVX-2 versus scalar (regular `for` loops we all know) for calculating the sum of larges values (3 millions elements computed 100 times).

I have never used SIMD. I was intrigued by this piece of hardware that can increase your performances more than tenfold and most of us software engineers don't know !

# How to run ?
`g++ -O2 -march=native a.cpp -o avx && ./avx`

You need AVX 2 support. You can customize the values of the macros at the top of the file (`SIZE_256` and `NUMBER_OF_SUM_RUN`)

# Results ?
On my Ryzen 6800U (decent CPU for laptop) computing 100 times the sum of 3 millions 256 bits elements took :
- -O0: 5080ms without SIMD, 1687ms with SIMD
- -O2: 2437ms without SIMD, 507ms with SIMD
- -O3: 575ms without SIMD, 858ms with SIMD

# Conclusions ?
- If I read that something is really hard I will do it (even when it means using gentoo for a year :( ).
- Hard to beat -O3 if you don't know what you're doing.
- But still so many people don't use -O3 because it lacks reliability so a nearly 5x increase in performances is still outstanding for -O2 ! And you can get much more on AVX-512, if you use smaller than 32 bits data and if you know what you're doing (unlike me)
- SIMD is not impossible to do but there's barely any good documentation (same as writing in assembly, or writing a Kernel) and it works quite differently than regular code. Not very hard but not intuitive.
- Compilers can't always unroll loops and most devs don't know how to use SIMD. So we sit with massively unused performances ?
- I don't think we have abstractions over SIMD instructions. So we have to learn every instructions of every family of CPU for max performances ?
