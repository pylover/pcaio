# pcaio
*Preemptive C Asynchronous Input Output*

I was trying to simulate [coroutines](https://en.wikipedia.org/wiki/Coroutine) 
in the C language for about three years. Yes, pause the C function at a point, 
wait for an IO event, allow the thread to do something else, and resume it 
when the data becomes available. just like the high-level languages 
`async/await` facility, such as `Python`, `Go`, etc.

It helps to wrap the `epoll(7)`, `poll(2)`, `select(2)` and `io_uring(7)` 
functionalities with more readable and understandable code structure and, of 
course, more syntactic sugar.

Recent experiences were derived from `Haskell`'s Monad and Arrow concepts, and 
also the C native 
[Duff's device](https://en.wikipedia.org/wiki/Duff%27s_device) way. You 
may check them out at these repositories:

- [meloop](https://github.com/pylover/meloop)
- [carrow](https://github.com/pylover/carrow)
- [caio](https://github.com/pylover/caio)

But after I found the `setcontext(3)`, `getcontext(3)`, `swapcontext(3)`  and 
`makecontext(3)` standard functions (which allow creating, retrieving and 
switching separate stacks), I decided to start this project.

Look at this pseudocode, and assume the wait statement is not blocking 
anymore. Instead, pause the function call, and return to the main loop to 
allow the thread to continue processing other tasks until the fd is reported 
as ready via the IO monitoring system/standard calls mentioned above.

```C
void
readA(int fd, char *buff, size_t count) {
    ...
retry:
    bytes = read(fd, buff, count);
    if ((bytes == -1) && (errno == EAGAIN)) {
        IOWAIT(fd, IOEV_READ);  // pause/resume at/from this point.
        goto retry;
    }
    ...
}
```

Check out the examples directory to figure out the API and how it works.

## Projects using pcaio
- [chttpd](https://github.com/chttpd/chttpd): POSIX compatible simple and Fast 
    application embedded HTTP server using the 
    [pcaio](https://github.com/pylover/pcaio).
