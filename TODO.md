schedule
- push a task into the queue.


loop
- pop a task form queue.
- feed it using some cpu times within a worker thread.
  - task calls one of iamok(), iamdone() functions to switch to the thread's
    main context.
- push the task into the queue's tail.
- start over
