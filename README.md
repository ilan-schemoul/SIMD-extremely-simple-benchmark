# What is this ?
This code compares the performances of AVX-2 versus scalar (regular `for` loops we all know) for calculating the sum of larges values (3 millions elements computed 100 times).

I did not know how to use SIMD but I knew it can really help performances and I also wanted to compare how efficiency of data structures (structure of array vs array of structure) can impact performances. So this is my experimentation to compare data structures and the use of SIMD.

# How to run ?
`g++ -O2 -march=native main.cpp -o avx && ./avx`

You need AVX 2 support. You can customize the values of the macros at the top of the file (`SIZE_256` and `NUMBER_OF_SUM_RUN`)

# Results ?
I run these tests on my Ryzen 6800U (decent CPU for laptop). I computed the sum of 24 millions 32 bits integer a 100 times in a row.

## -02
Array of structures \
Initialization=93ms sum=2399ms

Structure of arrays without SIMD \
Initialization=73ms sum=842ms

Structure of arrays with SIMD \
Initialization=61ms sum=551ms

## -03
Array of structures \
Initialization=86ms sum=592ms

Structure of arrays without SIMD \
Initialization=101ms sum=327ms

Structure of arrays with SIMD \
Initialization=105ms sum=601ms

# Conclusions ?

## SIMD
- -O3 is faster because it can unroll the loop better than I do. But better data structure still help it a lot.
- Compiler cannot always unroll the loop and -O2 is more reliable than -O3 so writing SIMD instructions can still make sense.
- I don't think we have abstractions over SIMD instructions. So we have to learn every instructions of every family of CPU for max performances.
- SIMD is not impossible to do but there's barely any comprehensive and friendly documentation (same as writing in assembly, or writing a Kernel) and it works quite differently than regular code. As I already said probably focus on data structures and check if your compiler can help you with SIMD.

## Efficiency of the structure of array
 Compiler can unroll the loop (sometimes) but can't transform your data structures (AFAIK). Writing a structure of array instead of an array of structure (check the code to understand what it means) can bring massive performances (3x for O2 in my case) in both O2 and O3.

If you have a structure containing x y z an array of this structure will look like this in your RAM |xyzxyzxyzxyzxyz|. \
When CPU accesses RAM it doesn't just load that one element you asked for but also a few KB/MB of the elements after that. But if you only access x in |xyzxyzxyzxyz| you are wasting 2/3 of your cache (so your CPU has to make 3 times more call to the RAM !) \
That's why you need a struct of array (`struct { int x[250]; int y[250]; int z[250] })`) so all "x" "y" and "z" are stored contiguously. In RAM it looks like this |xxxxxxxxxx| and when the CPU loads a part of the RAM in its cache it can fully utilize it (in this case we need to make 3 times less calls to RAM !). \
Of course this only applies when you only access one field of a struct but I do not think it is an infrequent case.
