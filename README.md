# simple object pool

An fixed size allocator.
**It is slower than malloc (glibc's implement) in most of cases.**

## Performance
```
malloc  alloc: 0.244640 s.  free: 0.068025 s, total: 0.312665 s.
mpool   alloc: 0.088866 s.  free: 0.169524 s, total: 0.258390 s.
```

* The free operation in sop is slower than free(). Because sop_free() use division operator to get the index and the
 operator is extremely slow for many platforms.
