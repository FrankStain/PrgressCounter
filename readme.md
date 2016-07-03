# Progress Counter
The Progress Counter is a lightweight tool class for counting the progress of some random amount of tasks.

### Description
Let's imagine you have a visual progress bar which displays some of next: progress of connection with network server, massive operations with files on hard disk, loading the game level; the reasonably question is how smoothly display the progress of such long tasks. More than, the real number of tasks and its complexity is real unknown, it different at any time. But you wish to display the smooth progress in this situation.

*"It will be great to design some tiny object, like `std::shared_ptr`, which may be placed into task and notify me that task is finished on its destructor!"*

Indeed! The `ProgressCounter` is designed for such situations. The instance of `ProgressCounter` (just *counter*) can produce the *goals* (aka instances of `ProgressGoal`). When the *goal* is attempted to be destroyed, it notifies the its host *counter* to update the state of progress. User can produce as much *goals* as he need using any of two ways: direct production from *counter* or proxied production from any *goal*.

### Thread safety
Internally it based on `std::shared_ptr`, so, if you believe to the description of standard, using of goals is thread-safe.
More than, the operations with progress counter are secured with critical section, what make entire work to be thread-safe.

### Examples
Inside of "/tests/test-concurrency/" folder the example of concurrency counting is placed.

The minimum what user have to do is to allocate the counter:

```.cpp
auto counter = ps::ProgressCounter::Create();
```

After that user only have to allocate some goals:

```.cpp
auto goal = counter->ProduceGoal();
auto goal2 = goal->GetHostCounter()->ProduceGoal();
```

That is all. Polling of counter will give the value of progress. Each time the goal is destroyed, the value of progress will be updated in the counter.


### Future improvements
Indeed, the current version of code is far from ideal. It uses allocation on heap, it needs the critical section, it may be improved by using more features of c++14, smth else?
So here the future plans:
* Get rid of `SharedProgressGoal`, make the *goal* to be safely movable without affecting the progress.
* Use more features of c++14
* Use the `atomic` values and getting rid of critical section.
