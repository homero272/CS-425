# Results for Assignment 01

## Improvement Iterations

Here's a table showing the improvements I did to make the application go faster.  The **Time** column in the table represents the _wall-clock time_ for a program run.

| Version | Time | Speedup | Memory (KB) | Changes |
| :-----: | ---- | :-----: | :------: | ------- |
| [01](test.cpp) | 3.66s | &mdash; | 1041752 | Initial version - no changes |
| 02 | 1.78s | 2.05x | 1041916 | Compiled with -Os to see about minimizing memory usage |
| [03](03.cpp) | 3.67 | .99x| 1041844 | using reciprocals as compared to divisions

## Profiling Analysis

### Initial Review

Looking at [01's profile](01.prof), the hottest function was `Transform::float4::perspectiveDivide() const`, which consumed around 28% of the program's execution time.  There's not a lot in that function, but it does three floating-point divisions, so perhaps that's something to try optimizing.

### Trying to make `computePerimeter()` go faster

computePerimeter()` does several divisions by the same value `w`.  A common trick is instead of dividing by the same value multiple times, to compute the value's reciprocal, and multiple by that value.  This assumes that multiplication is a faster operation than division.