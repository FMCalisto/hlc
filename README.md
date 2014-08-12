Heavy Light Decomposition

=========================


This repository presents a technique called Heavy Light Decomposition of a tree.


=========================


Definition

The heavy-light decomposition of a tree T = (V,E) is a coloring of the tree's edges. Each edge is either heavy or light. To determine which, consider the edge's two endpoints: one is closer to the root, and one is further away. If the size of the subtree rooted at the latter is more than half that of the subtree rooted at the former, the edge is heavy. Otherwise, it is light.


=========================


Basic Idea

Consider a node x. Let x have children u and v.

The edge x-u is called heavy if size(u) > 1/2 * size(x), otherwise it is called a light edge.

If a node has many children, then it is easy to note that at the most there can only be one heavy edge. It can be shown as follows:

Let the node be z and its children be u1,u2,….

Now,let the edge z-u1 and z-u2 be the heavy edges.

=> size(u1) > 1/2 * size(z) and size(u2) > 1/2 * size(z)
=> size(u1) + size(u2) > size(z)

which is a contradiction as size(u1) + size(u2) + size(u3) + … <= size(z)

Hence, there can at the most only one heavy edge connecting a node to its children.

So, if we look at a node, there can be at the most two heavy edges it may be connected to. One with its parent and the other with one of its children.


=========================


Light Edges

Considering the light edges, suppose if the edge x-y is light. Therefore, size(y) <= 1/2 * size(x), which means whenever we follow a light edge, the size of the tree is atleast halved. As a result, the number of light edges is O(lg n).


=========================


Building the Heavy Light Decomposition

The heavy light decomposition can be built using dfs. Pseudo Code for the same is given below.

procedure DFS(G,v):
       label v as discovered
       for all edges from v to w in G.adjacentEdges(v) do
          if vertex w is not labeled as discovered then
              recursively call DFS(G,w)
              
              
=========================


Ancestors and Lowest Common Ancestor

One of the major applications of the heavy light decomposition is the Lowest Common Ancestor queries.

Suppose if we want to find the whether a node is and ancestor of another node. This can be easily done using the time_in and time_out arrays computed in the above DFS code.time_in entry for a node gives the time when the node is first discovered and the time_out entry determines the time when all of its children have been explored. Let the nodes be x and y. The following condition checks if x is an ancestor of y:

if time_in[x] < time_in[y] and time_out[x] > time_out[y]
then print x is an ancestor of y

If we consider a Heavy Light Decomposition of a tree, then there will be chains heavy edges and then we will have the light edges. We can completely skip the heavy edges. Below is the pseudocode for the LCA problems which uses the above described ideas.
