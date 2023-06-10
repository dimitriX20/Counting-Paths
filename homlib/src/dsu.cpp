#include <vector>

struct DSU {
	std::vector<int> e;
	DSU(int N) { e = std::vector<int>(N, -1); }
    DSU() = default; // Default constructor

	// get representive component (uses path compression)
	int get(int x) { return e[x] < 0 ? x : e[x] = get(e[x]); }

	bool same_set(int a, int b) { return get(a) == get(b); }

	int size(int x) { return -e[get(x)]; }

	bool unite(int x, int y) {  // union by size
		x = get(x), y = get(y);
		if (x == y) return false;
		if (e[x] > e[y]) std::swap(x, y);
		e[x] += e[y];
		e[y] = x;
		return true;
	}
};