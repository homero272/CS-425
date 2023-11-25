# Results for Assignment 03

## Improvement Iterations

Here's a table showing the improvements I did to make the application go faster.  The **Finish Time** column in the table represents the finish time  for a website to finish loading.

| Version | Time | Speedup | Changes |
| :-----: | ---- | :-----: | ------- |
| [01](server.cpp) | 1.8m | &mdash; | Initial version - no changes |
| [02](server1.cpp) | 1.7m | 1.06x | Implemented async tasks to the requests |
| [03](server2.cpp) | 1.4m | 1.29x | Implmented ring buffer following producers/consumers to increase performnace|

## Profiling Analysis
**Disclaimer** Finish time varies from network to network, and for an odd reason doing these tests at home with about a 
250mbps wifi speed, took longer than at Sonoma State. In order to compare times, all tests were done at home. The number of threads that chrome allows is also a limiting factor.

### Initial Review
There were a lot of images that were being loaded into the server, and that caused a lot of traffic because it was being done iteraitvely. As introuduced in class, asynchronous tasks would help thread this web server and allow for all the images to be loaded in faster.

### Trying to thread `main()`to go faster

It took a long time with no changes to load all the images into the webserver (1.8m) in the initial run. When packaging the part of the code where the requests are created and sent to the server into an async task, it minimally boosted the perfromance and finish time by about 1.06x.

### Trying to thread `main()`to go faster different approach
After incorporating a ring buffer, there was a slightly smaller improvment of 1.29x. It seemed to be a better solution to trying to make this web server faster.


### Changing compile flags
By changing the compile flag to -O3, in combination to the previous threading and ring buffer , the best speed up was with the ring buffer implementation with a total speed up of 1.29x.
