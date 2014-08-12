
/*
 *
 *	Heavy Light Decomposition
 *
 *	This function transform a Graph into Trees
 *	to detect cycles so that it can find words of a file
 *	in Logarithmic time.
 *
 *	For exemple, if the solution of a problem needs a
 *	function to change the weight of a group of a nodes
 *	to do some kind of sum queries from all nodes
 *	in a path.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

#define DEBUG 0
#define INF 2000000000
#define NMAX 111111
#define MMAX 222222

struct Edge {
	int a, b, cap, cycle, poz_in_cycle, hpdnode, poz_in_hpdnode;
};

vector<int> vecedge[NMAX];
struct Edge edge[MMAX];
int N, M;

void ReadInput() {
	int k;
	scanf("%d %d", &N, &M);
	for (k = 1; k <= M; k++) {
		scanf("%d %d %d", &edge[k].a, &edge[k].b, &edge[k].cap);
		vecedge[edge[k].a].push_back(k);
		vecedge[edge[k].b].push_back(k);
	}
}

#define MAXNODES 10000000

int vmin[MAXNODES], lson[MAXNODES], rson[MAXNODES], li[MAXNODES], ls[MAXNODES];
int nnodes;

struct Cycle {
	vector<int> nodes, edges, child_cycle_edges;
	int seg_tree_root, parent_cycle, hpdnode, poz_in_hpdnode, subtree_size, num_segtree_leaves;
	int hpdnode_in_edge, hpdnode_out_edge;
};

struct Cycle cycle[NMAX];
int ncycles;

struct Node {
	int cycle, poz_in_cycle;
};

struct Node node[NMAX];

int parent[NMAX], parentedge[NMAX];
char visited[NMAX];

void MakeCycle(int na, int nb, int ledge) {
	ncycles++;

	while (nb != na) {
		node[nb].cycle = ncycles;
		node[nb].poz_in_cycle = cycle[ncycles].nodes.size();
		edge[parentedge[nb]].cycle = ncycles;
		edge[parentedge[nb]].poz_in_cycle = cycle[ncycles].edges.size();
		cycle[ncycles].nodes.push_back(nb);
		cycle[ncycles].edges.push_back(parentedge[nb]);
		nb = parent[nb];		
	}

	node[na].cycle = ncycles;
	node[na].poz_in_cycle = cycle[ncycles].nodes.size();
	edge[ledge].cycle = ncycles;
	edge[ledge].poz_in_cycle = cycle[ncycles].edges.size();
	cycle[ncycles].nodes.push_back(na);
	cycle[ncycles].edges.push_back(ledge);

	// Alloc nodes for the cycle's seg-tree.
	int needed_nodes = 2;
	while (needed_nodes < cycle[ncycles].edges.size())
		needed_nodes <<= 1;
	if (nnodes + 2 * needed_nodes >= MAXNODES) exit(1);
	
	int i;
	for (i = 0; i < needed_nodes; i++) {
		li[nnodes + needed_nodes + i] = ls[nnodes + needed_nodes + i] = i;
		if (i < cycle[ncycles].edges.size())
			vmin[nnodes + needed_nodes + i] = edge[cycle[ncycles].edges[i]].cap;
		else
			vmin[nnodes + needed_nodes + i] = INF;
	}

	for (i = needed_nodes - 1; i >= 1; i--) {
		lson[nnodes + i] = nnodes + 2 * i;
		rson[nnodes + i] = nnodes + 2 * i + 1;
		li[nnodes + i] = li[lson[nnodes + i]];
		ls[nnodes + i] = ls[rson[nnodes + i]];
		if (vmin[lson[nnodes + i]] <= vmin[rson[nnodes + i]])
			vmin[nnodes + i] = vmin[lson[nnodes + i]];
		else
			vmin[nnodes + i] = vmin[rson[nnodes + i]];
	}

	cycle[ncycles].seg_tree_root = nnodes + 1;
	cycle[ncycles].num_segtree_leaves = needed_nodes;
	nnodes += 2 * needed_nodes;

	cycle[ncycles].subtree_size = 1;
	
	if (DEBUG) {
		fprintf(stderr, "Cycle %d (nleaves=%d):", ncycles, cycle[ncycles].num_segtree_leaves);
		for (i = 0; i < cycle[ncycles].nodes.size(); i++)
			fprintf(stderr, " %d(%d)", cycle[ncycles].nodes[i], cycle[ncycles].edges[i]);
		fprintf(stderr, "\n");
	}
}

void DFS(int x) {
	visited[x] = 1;
	node[x].cycle = 0;
	for (int z = 0; z < vecedge[x].size(); z++) {
		int y = vecedge[x][z];
		if (edge[y].a == x) y = edge[y].b;
		else y = edge[y].a;
		if (!visited[y]) {
			parent[y] = x;
			parentedge[y] = vecedge[x][z];
			DFS(y);
		} else if (y != parent[x] && node[x].cycle == 0)
			MakeCycle(y, x, vecedge[x][z]);
	}

	if (node[x].cycle == 0)
		MakeCycle(x, x, 0);

	for (int z = 0; z < vecedge[x].size(); z++) {
		int y = vecedge[x][z];
		if (edge[y].a == x) y = edge[y].b;
		else y = edge[y].a;
		if (node[y].cycle != node[x].cycle && y != parent[x]) {
			cycle[node[x].cycle].child_cycle_edges.push_back(vecedge[x][z]);
			cycle[node[y].cycle].parent_cycle = node[x].cycle;
			cycle[node[x].cycle].subtree_size += cycle[node[y].cycle].subtree_size;
		}
	}
}

int QA, QB, QOP, QU, QANS;

int LEV;

void SegTreeOp(int node) {
	if (QA > QB || node == 0 || QA > ls[node] || QB < li[node]) exit(1);

	if (QA <= li[node] && ls[node] <= QB) {
		if (QOP == 0) {
			if (vmin[node] < QANS) QANS = vmin[node];
		} else {
			vmin[node] = QU;
		}
		return;
	}

	LEV++;
	
	if (QA <= ls[lson[node]])
		SegTreeOp(lson[node]);
	if (QB >= li[rson[node]])
		SegTreeOp(rson[node]);
	
	if (QOP == 1) {
		if (vmin[lson[node]] <= vmin[rson[node]])
			vmin[node] = vmin[lson[node]];
		else
			vmin[node] = vmin[rson[node]];
	}

	LEV--;
}

int GetCycleCap(int c, int start, int end) {
	start = node[start].poz_in_cycle;
	end = node[end].poz_in_cycle;
	if (start > end) {
		int tmp = start;
		start = end;
		end = tmp;
	}

	if (start == end) return INF;

	int C1, C2;
	QOP = 0;

	// Query the cycle's segment tree.
	QA = start; QB = end - 1; QANS = INF;
	SegTreeOp(cycle[c].seg_tree_root);
	C1 = QANS;

	QA = end; QB = cycle[c].num_segtree_leaves - 1; QANS = INF;
	if (QA <= QB) SegTreeOp(cycle[c].seg_tree_root);
	QA = 0; QB = start - 1;
	if (QA <= QB) SegTreeOp(cycle[c].seg_tree_root);
	C2 = QANS;

	if (DEBUG) fprintf(stderr, "[GetCycleCap c=%d start=%d end=%d] C1=%d C2=%d total=%d\n", c, start, end, C1, C2, C1 + C2);
	return C1 + C2;
}

int GetHPDNodeCycleCap(int c) {
	if (!cycle[c].hpdnode_in_edge || !cycle[c].hpdnode_out_edge)
		return INF;
	int start, end;
	if (node[edge[cycle[c].hpdnode_in_edge].a].cycle == c)
		start = edge[cycle[c].hpdnode_in_edge].a;
	else
		start = edge[cycle[c].hpdnode_in_edge].b;
	if (node[edge[cycle[c].hpdnode_out_edge].a].cycle == c)
		end = edge[cycle[c].hpdnode_out_edge].a;
	else
		end = edge[cycle[c].hpdnode_out_edge].b;
	return GetCycleCap(c, start, end);
}

struct HPDNode {
	vector<int> cycles, edges;
	int cycles_seg_tree_root, edges_seg_tree_root, pathp, pathpcycle, pedge, num_segtree_leaves, level;
};

struct HPDNode hpdnode[NMAX];

int hpdq[NMAX][5], hpdli, hpdls;
int pathp[NMAX], pathpnode[NMAX], nodepath[NMAX], nodepozinpath[NMAX];
int num_hpd_nodes;

void BuildHPDNode(int rcycle, int pathparent, int pathparentcycle, int pedge, int level) {
	num_hpd_nodes++;
	hpdnode[num_hpd_nodes].pathp = pathparent;
	hpdnode[num_hpd_nodes].pathpcycle = pathparentcycle;
	hpdnode[num_hpd_nodes].pedge = pedge;
	hpdnode[num_hpd_nodes].level = level;

	int i = rcycle, j, k, nexti;
	hpdnode[num_hpd_nodes].cycles.push_back(i);
	cycle[i].hpdnode = num_hpd_nodes;
	cycle[i].poz_in_hpdnode = hpdnode[num_hpd_nodes].cycles.size() - 1;
	cycle[i].hpdnode_in_edge = pedge;

	while (1) {
		nexti = 0;
		for (k = 0; k < cycle[i].child_cycle_edges.size(); k++) {
			j = cycle[i].child_cycle_edges[k];
			if (node[edge[j].a].cycle == i) j = node[edge[j].b].cycle;
			else j = node[edge[j].a].cycle;
			if (!nexti || cycle[j].subtree_size > cycle[nexti].subtree_size)
				nexti = j;
		}

		if (!nexti)
			break;
		
		int nextedge = 0;

		for (k = 0; k < cycle[i].child_cycle_edges.size(); k++) {
			j = cycle[i].child_cycle_edges[k];
			if (node[edge[j].a].cycle == i) j = node[edge[j].b].cycle;
			else j = node[edge[j].a].cycle;
			if (j != nexti) {
				hpdls++;
				hpdq[hpdls][0] = j;
				hpdq[hpdls][1] = num_hpd_nodes;
				hpdq[hpdls][2] = i;
				hpdq[hpdls][3] = cycle[i].child_cycle_edges[k];
				hpdq[hpdls][4] = level + 1;
			} else
				nextedge = cycle[i].child_cycle_edges[k];
		}

		cycle[i].hpdnode_out_edge = cycle[nexti].hpdnode_in_edge = nextedge;

		i = nexti;
		hpdnode[num_hpd_nodes].cycles.push_back(i);
		cycle[i].hpdnode = num_hpd_nodes;
		cycle[i].poz_in_hpdnode = hpdnode[num_hpd_nodes].cycles.size() - 1;

		hpdnode[num_hpd_nodes].edges.push_back(nextedge);
		edge[nextedge].hpdnode = num_hpd_nodes;
		edge[nextedge].poz_in_hpdnode = hpdnode[num_hpd_nodes].edges.size() - 1;
	}

	// Alloc nodes for the hpdnode's edges.
	int needed_nodes = 2;
	while (needed_nodes < hpdnode[num_hpd_nodes].cycles.size())
		needed_nodes <<= 1;

	if (nnodes + 4 * needed_nodes >= MAXNODES) exit(1);

	for (i = 0; i < needed_nodes; i++) {
		li[nnodes + needed_nodes + i] = ls[nnodes + needed_nodes + i] = i;
		if (i < hpdnode[num_hpd_nodes].edges.size())
			vmin[nnodes + needed_nodes + i] = edge[hpdnode[num_hpd_nodes].edges[i]].cap;
		else
			vmin[nnodes + needed_nodes + i] = INF;
	}

	for (i = needed_nodes - 1; i >= 1; i--) {
		lson[nnodes + i] = nnodes + 2 * i;
		rson[nnodes + i] = nnodes + 2 * i + 1;
		li[nnodes + i] = li[lson[nnodes + i]];
		ls[nnodes + i] = ls[rson[nnodes + i]];
		if (vmin[lson[nnodes + i]] <= vmin[rson[nnodes + i]])
			vmin[nnodes + i] = vmin[lson[nnodes + i]];
		else
			vmin[nnodes + i] = vmin[rson[nnodes + i]];
	}

	hpdnode[num_hpd_nodes].edges_seg_tree_root = nnodes + 1;
	hpdnode[num_hpd_nodes].num_segtree_leaves = needed_nodes;
	
	nnodes += 2 * needed_nodes;

	// Alloc nodes for the hpdnode's cycles.
	for (i = 0; i < needed_nodes; i++) {
		li[nnodes + needed_nodes + i] = ls[nnodes + needed_nodes + i] = i;
		if (i < hpdnode[num_hpd_nodes].cycles.size())
			vmin[nnodes + needed_nodes + i] = GetHPDNodeCycleCap(hpdnode[num_hpd_nodes].cycles[i]);
		else
			vmin[nnodes + needed_nodes + i] = INF;
	}

	for (i = needed_nodes - 1; i >= 1; i--) {
		lson[nnodes + i] = nnodes + 2 * i;
		rson[nnodes + i] = nnodes + 2 * i + 1;
		li[nnodes + i] = li[lson[nnodes + i]];
		ls[nnodes + i] = ls[rson[nnodes + i]];
		if (vmin[lson[nnodes + i]] <= vmin[rson[nnodes + i]])
			vmin[nnodes + i] = vmin[lson[nnodes + i]];
		else
			vmin[nnodes + i] = vmin[rson[nnodes + i]];
	}

	hpdnode[num_hpd_nodes].cycles_seg_tree_root = nnodes + 1;
	hpdnode[num_hpd_nodes].num_segtree_leaves = needed_nodes;
	
	nnodes += 2 * needed_nodes;
	
	if (DEBUG) {
		fprintf(stderr, "HPDNode %d (pathp=%d pathpcycle=%d nleaves=%d):", num_hpd_nodes, hpdnode[num_hpd_nodes].pathp, hpdnode[num_hpd_nodes].pathpcycle, hpdnode[num_hpd_nodes].num_segtree_leaves);
		for (i = 0; i < hpdnode[num_hpd_nodes].cycles.size(); i++) {
			fprintf(stderr, " %d/%d", hpdnode[num_hpd_nodes].cycles[i], cycle[hpdnode[num_hpd_nodes].cycles[i]].poz_in_hpdnode);
			if (i < hpdnode[num_hpd_nodes].edges.size())
				fprintf(stderr, "(%d/%d)", hpdnode[num_hpd_nodes].edges[i], edge[hpdnode[num_hpd_nodes].edges[i]].poz_in_hpdnode);
		}
		fprintf(stderr, "\n");
	}
}

void BuildHPD() {
	num_hpd_nodes = 0;
	hpdli = hpdls = 0;
	hpdq[0][0] = node[1].cycle;
	hpdq[0][1] = hpdq[0][2] = hpdq[0][3] = hpdq[0][4] = 0;
	while (hpdli <= hpdls) {
		BuildHPDNode(hpdq[hpdli][0], hpdq[hpdli][1], hpdq[hpdli][2], hpdq[hpdli][3], hpdq[hpdli][4]);
		hpdli++;
	}
}

int GoUp(int X, int& ans) {
	int c = node[X].cycle;
	int h = cycle[c].hpdnode;
	int inedge = cycle[c].hpdnode_in_edge;
	int Y;
	if (node[edge[inedge].a].cycle == c)
		Y = edge[inedge].a;
	else
		Y = edge[inedge].b;
	int capxy = GetCycleCap(c, X, Y);
	if (capxy < ans) ans = capxy;
	
	QA = 0; QB = cycle[c].poz_in_hpdnode - 1;
	if (QA <= QB) {
		QANS = INF;
		SegTreeOp(hpdnode[h].cycles_seg_tree_root);
		SegTreeOp(hpdnode[h].edges_seg_tree_root);
		if (QANS < ans) ans = QANS;
	}

	inedge = hpdnode[h].pedge;
	if (edge[inedge].cap < ans) ans = edge[inedge].cap;
	if (cycle[node[edge[inedge].a].cycle].hpdnode == h)
		return edge[inedge].b;
	return edge[inedge].a;
}

int QueryHPD(int S, int T) {
	int ans = INF;

	while (cycle[node[S].cycle].hpdnode != cycle[node[T].cycle].hpdnode) {
		if (hpdnode[cycle[node[S].cycle].hpdnode].level >=
			hpdnode[cycle[node[T].cycle].hpdnode].level) {
			S = GoUp(S, ans);
		} else {
			T = GoUp(T, ans);
		}
	}

	if (S != T) {
		int cs = node[S].cycle, ct = node[T].cycle;
		int h = cycle[cs].hpdnode;
		if (cycle[cs].poz_in_hpdnode > cycle[ct].poz_in_hpdnode) {
			int tmp = cs; cs = ct; ct = tmp;
			tmp = S; S = T; T = tmp;
		}

		if (cs == ct) {
			int cap = GetCycleCap(cs, S, T);
			if (cap < ans) ans = cap;
		} else {
			int inedge = cycle[cs].hpdnode_out_edge;
			int Y;
			if (node[edge[inedge].a].cycle == cs)
				Y = edge[inedge].a;
			else
				Y = edge[inedge].b;
			int cap = GetCycleCap(cs, S, Y);
			if (cap < ans) ans = cap;
			
			inedge = cycle[ct].hpdnode_in_edge;
			if (node[edge[inedge].a].cycle == ct)
				Y = edge[inedge].a;
			else
				Y = edge[inedge].b;
			cap = GetCycleCap(ct, T, Y);
			if (cap < ans) ans = cap;
			
			QANS = INF;
			QA = cycle[cs].poz_in_hpdnode; QB = cycle[ct].poz_in_hpdnode - 1;
			if (QA <= QB) SegTreeOp(hpdnode[h].edges_seg_tree_root);
			QA++;
			if (QA <= QB) SegTreeOp(hpdnode[h].cycles_seg_tree_root);
			if (QANS < ans) ans = QANS;
		}
	}
	
	return ans;
}

void ProcessOps() {
	int Q, S, T, X;
	scanf("%d", &Q);

	while (Q--) {
		scanf("%d", &QOP);
		if (QOP == 0) {
			scanf("%d %d", &S, &T);
			if (DEBUG) fprintf(stderr, "[Query] S=%d T=%d\n", S, T);
			printf("%d\n", QueryHPD(S, T));
		} else {
			scanf("%d", &X);
			scanf("%d", &edge[X].cap);
			if (DEBUG) fprintf(stderr, "[Update] X=%d newcap=%d\n", X, edge[X].cap);
			if (edge[X].cycle >= 1) {
				int c = edge[X].cycle;
				QA = QB = edge[X].poz_in_cycle;
				QU = edge[X].cap;
				if (DEBUG) fprintf(stderr, "  update edge in cycle %d: QA=%d QB=%d QU=%d\n", c, QA, QB, QU);
				SegTreeOp(cycle[edge[X].cycle].seg_tree_root);
				QU = GetHPDNodeCycleCap(c);
				QOP = 1;
				QA = QB = cycle[c].poz_in_hpdnode;
				if (DEBUG) fprintf(stderr, "  update cycle %d cap in hpdnode %d: QA=%d QB=%d QU=%d\n", c, cycle[c].hpdnode, QA, QB, QU);
				SegTreeOp(hpdnode[cycle[c].hpdnode].cycles_seg_tree_root);
			} else if (edge[X].hpdnode >= 1) {
				QA = QB = edge[X].poz_in_hpdnode;
				QU = edge[X].cap;
				SegTreeOp(hpdnode[edge[X].hpdnode].edges_seg_tree_root);
			}
		}
	}
}

int main() {
//	freopen("x.txt", "r", stdin);
	ReadInput();
	ncycles = nnodes = 0;
	edge[0].cap = INF;
	DFS(1);
	BuildHPD();
	ProcessOps();
	return 0;
}
