//#include "subgraphCounting.cpp"
//#include "testGraph.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

int main() {
    int n, m, start, end, k;
    std::vector<std::vector<bool>> g;
    std::string line;
    bool terminals = false;

    while(std::getline(std::cin, line)) {
        if(line.empty()) 
			continue;  

        char line_type = line[0];
        std::istringstream iss(line.substr(1)); // remove line_type and parse the rest

        if (line_type == 'c') {
            
        } else if (line_type == 'p') {
            std::string problem_type;
            iss >> problem_type >> n >> m;
			g.resize(n, std::vector<bool>(n, false));

        } else if (line_type == 'e') {
            int v1, v2;
            iss >> v1 >> v2;
            v1--, v2--; 
            g[v1][v2] = g[v2][v1] = true;
        } else if (line_type == 'l') {
            iss >> k; 
            k += 1;
        } else if (line_type == 't') {
            iss >> start >> end;
            start--, end--; 
            terminals = true;
        }
    } 

	if (terminals) {
		int64_t ans = 0; 

		std::vector<std::vector<int64_t>> dp(1 << n, std::vector<int64_t>(n, 0LL));
		dp[1 << start][start] = 1;
		for (int s = (1 << start); s < (1 << n); s++) {
			if (__builtin_popcount(s) >= k) 
				continue;

			for (int i = 0; i < n; i++) { 
				if (dp[s][i]) {
					for (int j = 0; j < n; j++) {
						if (g[i][j] && (~s >> j & 1)) {
							dp[s | 1 << j][j] += dp[s][i];
							ans += (j == end) * dp[s][i]; 
						}
					}
				}
			}
		}
		
		
		std::cout << ans << "\n";
	}
    
    return 0;
}