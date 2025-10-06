# mpi-mutation-tool

> Mutates MPI codes to introduce MPI usage errors.
> 
build it with cmake : `mkdir build && cd build && cmaker .. && make`


## Example: generate a mutation

```bash
# usage (example)
cp test_file.c test_file.c.old
build/MPIMutationTool -extra-arg="-I/your_mpi_installation/include" -m ArgErrorNullBufferMutation test_file.c
diff test_file.c test_file.c.old
```
* The Mutation tool works in-place, so you need to copy the source file, if you want to compare
* The `-extra-arg=` set additional compiler flags you probably need to specify all relevant include path for your MPI implementation
* you can choose the mutation to apply with `-m` see `available_mutations.txt` for a list of possible mutations.
* If the mutation is applicable in multiple places (e.g. to multiple MPI calls) you can choose which places are mutated with `-l` Possible locations are numbered in the order of appreance in the source code, you can specify multiple locations at the same time, for example: `build/MPIMutationTool -extra-arg="-I/your_mpi_installation/include" -m ArgErrorNullBufferMutation test_file.c -l 2 -l 4` will only set the buffer argument to `NULL` in the second and third MPI call in the source file, to count the number of applicable locations, use `-c`
* Also refer to `MPIMutationTool --help`

---

## Content of key files

- `src/` — implementation of the mutation engine and helpers. If you want to add new mutation kinds, this is where to add them.
- `available_mutations.txt` — short reference of mutation names and brief descriptions; use this as an index when scripting or selecting mutations programmatically.
- `patches/` — sample mutation patches that show the kinds of changes produced by the tool;
- `mutate_test_file.sh` — small example that demonstrates create-patch -> apply -> build flow for a single-file target.

- `wrapper.sh` — a helper script used to automates patch application + build + running with an MPi correctness checking tool.
- `evaluate_results.py` — helper for collecting detection results of MPI correctness checking tools.