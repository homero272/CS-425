# Results for Assignment 01

## Improvement Iterations

Here's a table showing the improvements I did to make the application go faster.  The **Time** column in the table represents the _wall-clock time_ for a program run.

| Version | Time | Speedup | Memory (KB) | Changes |
| :-----: | ---- | :-----: | :------: | ------- |
| [01](test.cpp) | 10.51s | &mdash; | 1044092 | Initial version - no changes |
| [02] (01.cpp) | 9.73s | 1.08x | 1041332 | Unrolled loop that was calling computePerimeter() |
| [03](01.cpp) | 1.97 | 5.33x| 1041332 | along with the loop unrolling, I compile with the -Ofast flag

## Profiling Analysis

### Initial Review

Looking at [test's profile], the hottest function was `computePerimeter()`, which consumed the most time as well as was called the most. I 

### Trying to make `computePerimeter()` go faster

computePerimeter()` is called a bunch of times. In order to hopefully help this issue, I unrolled the loop where computePerimeter() is defined. However, that was a bust and slowed things done. Next I located where `computePreimeter()`was called. I unrolled the loop there and by doing so it helped by almost 1 second. 

### Changing compile flags
By changing the compile flag to -Ofast, in combination to the previous change of unrolling loops, it significantly helped and sped up the process by about 5x.