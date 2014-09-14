# Quick Explanation

Firstly it is obvious that a connected graph on `N` vertices with `N` edges will contain exactly one cycle.

It is not necessary for that cycle to be an odd-length cycle; but the problem statement assures you that such a cycle will always be odd length. This constraint ensures that there is always a unique shortest path between any two vertices in the graph.

Let us explore an easy to state solution for this graph.

For either of the flip or find operations, we will move along the unique path in the graph, edge by edge, and update / calculate the result. The update will of course change the sign of the edge weight. The result can of course be calculated using Kadane's algorithm.

How do we exactly trace the shortest path between any two vertices? We use the following insights

The graph has a single cycle
Vertices of this cycle behave as roots of connected trees
Now, for each of the trees, we can orient them. This effectively means that for each vertex in the tree, we can mark its unique parent, right up to the root, which lies in the cycle. We of course do not mark the root of the tree.

For each vertex we can mark which tree it belongs to (probably just store the label of the root vertex of the tree). This helps us figure out for some given vertices u and v, whether they are in the same tree or not.

Now, we can trace the path from `u` to `v`

If they are in the same tree, we can find their `LCA`.
This will require calculating the depth for each vertex
If they are in separate trees
The path of both the vertices to their roots, say s and t respectively are to be considered
Then the path from s to t, through the cycle, should be considered
The path from the cycle should be the smaller one. This can be determined trivially by arranging the vertices in an array in a clockwise (or counter-clockwise) traversal of the vertices in the cycle.
The complextiy of each flip or find opeartion will be `O(N)`. As you can figure, this is too low.

Let us see how to make this `O(log2 N)`.
