# Results for Assignment 02

## Improvement Iterations

Here's a table showing the improvements I did to make the application go faster.  The **Time** column in the table represents the _wall-clock time_ for a program run.

| Version | Time | Speedup | Changes |
| :-----: | ---- | :-----: | ------- |
| [01](lychrel.cpp) | 11.48m | &mdash; | Initial version - no changes |
| [02](lychrel.cpp) | na | na | na |
| [03](lychrel.cpp) | na | na | na|

## Profiling Analysis

### Initial Review
There were a lot of iterations occurring and that for loop and while loop were taking a big toll on performance. We need to alleviate this issue via 
threads!!!

### Trying to thread `main()`to go faster

It took a long time with no changes to run the lychrel program. However, when introduced to threads this should make it a lot faster. Also
incorpoerating dynamic scheduling, helps performance by making the load balance better.
### Changing compile flags
By changing the compile flag to -Ofast, in combination to the previous threading and dynamic scheduling, it sped up the process by about
