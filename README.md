# HexThello AI Agent

This project implements an intelligent AI agent for the **HexThello** board game. It was developed as part of the course **PLH311 – Artificial Intelligence** at the **Technical University of Crete**.

The project is based on the official **TUC HexThello v0.98b** framework created by **Ioannis Skoulakis**, and extends the default player by implementing **Minimax** and **Alpha-Beta Pruning** algorithms.

---

## Project Base: TUC HexThello v0.98b

```
Author: Ioannis Skoulakis  
Affiliation: Technical University of Crete  
Version: v0.98b
```

Requirements:
- Linux (any distribution)
- gcc / make

Compilation:

```bash
make           # builds client and server
make client    # builds just the AI player
make server    # builds just the game server
```

Execution:

```bash
./guiServer    # graphical server
./server       # terminal-based server
./client -i IP -p PORT    # AI client connection
```

---

## Implemented Strategies

### Minimax

- Evaluates all possible moves up to a depth
- Uses: `V(state) = my_score - opponent_score`

### Alpha-Beta Pruning

- Optimized version of Minimax
- Uses α/β bounds to prune unnecessary paths
- Same results, faster performance


When running the client, you will be prompted to choose the agent:

- Type `1` → to activate **Minimax**
- Type `2` → to activate **Alpha-Beta Pruning**

---

## Acknowledgments

- **Prof. M. Lagoudakis** & **Ioannis Skoulakis** for course material and original HexThello base code    
- AIMA Pseudocode

