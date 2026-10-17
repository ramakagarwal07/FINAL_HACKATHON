# Dining Philosophers Deadlock Prevention System

## 1. Project Overview

The Dining Philosophers Deadlock Prevention System is an Operating Systems project developed in C using POSIX threads, mutexes, and semaphores.

It simulates five philosophers sharing five forks and demonstrates different methods of preventing deadlock and improving access to shared resources.

## 2. Objectives

- Understand deadlock and resource sharing.
- Demonstrate thread synchronization.
- Compare three synchronization strategies.
- Verify that all philosophers complete their meals.
- Explore FIFO ordering to reduce starvation.

## 3. Technologies Used

- Language: C
- Operating System: Ubuntu Linux / WSL
- Compiler: GCC
- Threads: POSIX pthreads
- Synchronization: Mutexes, semaphores, condition variables
- Build tool: GNU Make

## 4. Synchronization Strategies

### Strategy 1: Resource Ordering
Forks are acquired in a consistent numerical order. This prevents circular wait and therefore prevents deadlock.

### Strategy 2: Waiter / N-1 Semaphore
A semaphore permits at most four of the five philosophers to compete for forks at once. This prevents circular-wait deadlock in this implementation.

### Strategy 3: Fair FIFO Queue
Philosophers receive tickets and requests are handled in ticket order. This helps prevent later requests from overtaking earlier requests. Progress still depends on normal thread scheduling.

## 5. How to Run

Compile the program:

```bash
make
```

Run Strategy 1:

```bash
make run1
```

Run Strategy 2:

```bash
make run2
```

Run Strategy 3:

```bash
make run3
```

Remove the compiled executable:

```bash
make clean
```

## 6. Testing and Results

The prototype was tested with five philosophers and five meals per philosopher.

| Strategy | Expected meals | Observed meals | Result |
|---|---:|---:|---|
| Resource Ordering | 25 | 25 | PASS |
| Waiter Semaphore | 25 | 25 | PASS |
| Fair FIFO Queue | 25 | 25 | PASS |

All three test runs completed 25 meals in total, with each philosopher completing five meals.

These results demonstrate successful completion in the tested runs; they are not a formal proof of every possible scheduling outcome.

## 7. Limitations

- The current simulation uses a fixed number of philosophers and meals.
- Execution order varies between runs because threads execute concurrently.
- The FIFO strategy does not control the operating system's CPU scheduler.
- The project is a simulation for learning synchronization concepts.

## 8. Future Enhancements

- Add a graphical user interface.
- Measure waiting time and eating time.
- Compare strategies using repeated performance tests.
- Add configurable numbers of philosophers and meals.
- Display synchronization statistics.

## 9. Conclusion

The project demonstrates how mutexes, semaphores, and FIFO request ordering can coordinate concurrent threads sharing limited resources. The three strategies provide a practical way to study deadlock prevention and starvation-related concerns in Operating Systems.

