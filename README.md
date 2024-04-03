# What is this ?
This code compares the performances of AVX-2 versus scalar (regular `for` loops we all know) for calculating the sum of larges values (3 millions elements computed 100 times).

I have never used SIMD. It seemed unbearable most software engineers do not use a piece of hardware that can increase your performances more than tenfold !

# How to run ?
`g++ -O2 -march=native a.cpp -o avx && ./avx`

You need AVX 2 support. You can customize the values of the macros of the of the file.

# Results ?
On my Ryzen 6800U (relatively powerful CPU for laptop) computing the sum of 3 000 000 256 bits elements 100 times in a row took :
- -O0: 5080ms without SIMD, 1687ms with SIMD
- -O2: 2437ms without SIMD, 507ms with SIMD
- -O3: 575ms without SIMD, 858ms with SIMD

# Conclusions ?
- If I read that something is really hard I will do it (even when it means using gentoo for a year :( ).
- Hard to beat -O3 if you don't know what you're doing.
- But still so many people don't use -O3 because it lacks reliability so a nearly 5x increase in performances is still outstanding for -O2 ! And you can get much more on AVX-512 and if you use smaller than 32 bits data.
- SIMD is not impossible to do but there's barely not a lot of documentation (same as writing in assembly, or writing a Kernel) and it works quite differently than regular code. Not very hard but not intuitive.
- Lack of beginner friendly documentation when it means sitting on up to 64x performances is surprising (?!)
- I don't know why but I couldn't make this work for 8 bits. I don't know if it's hard to do but chances are performances improvement would be much more impressive.
- I am open to people who know what they are doing telling me it's hot garbage :)

