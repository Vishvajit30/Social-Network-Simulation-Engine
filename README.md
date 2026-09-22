# Social Network Simulation Engine

A backend network graph simulator implemented in C++ modeling relationship topologies, feed dynamics, and shortest-path connection discovery.

## Key Architecture
- **Undirected Graph Network**: Stores bilateral user friendships using associative Adjacency Lists.
- **Self-Balancing AVL Trees**: Manages user feeds using strict height balancing (LL, RR, LR, RL rotations), providing guaranteed $O(\log N)$ post lookups and updates.
- **Degrees of Separation**: Employs Breadth-First Search (BFS) shortest-path traversal to compute exact hop distances between arbitrary network nodes in $O(V + E)$ time.
- **Mutual-Friend Recommendation**: Automatically scores and suggests friends at 2nd-degree separation based on shared connections.

## Build and Run
```bash
g++ -std=c++17 SocialNetwork.cpp -o social_network
./social_network
