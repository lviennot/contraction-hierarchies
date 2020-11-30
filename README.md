# Contraction Hierarchies

C++ code for contracting a weighted graph according to Contraction Hierarchies (CH) algorithm. An optional argument allows to preserve a given set of nodes from contraction.

Compile with `make` (which will call cmake).

### Distance preserver

Given a weighted directed graph in a file named `graph.txt` (whose edges are given by triples `src dst len` on each line) and a subset of `n` nodes in a file `nodes.txt` (one node per line), a distance preserver is a graph restricted to nodes in the subset such that distances between these nodes are the same as in the original graph. Such a distance preserver can be obtained by:

```
make && _build/main graph.txt nodes.txt n
```

The last argument indicates to stop as soon as average degree reaches that value. If a value smaller than `n` is provided, the graph may contain additional nodes than those in the subset.


### Distanc oracle

For a distance oracle usage, see the second part of `src/benchmark.cc`.


### Acknowledgements

Thanks to André Nusser and David Coudert for showing nice tricks.


