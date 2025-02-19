#ifndef MCTS_H
#define MCTS_H

#include "board.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

// MCTS Tree Node
class MCTSNode {
private:
    AstraDoBoard board;
    MCTSNode* parent;
    std::vector<MCTSNode*> children;
    uint8_t move;           // Move that leads to this position

    int numVisits = 0;
    double winSum = 0;      // +1 if black wins, -1 if white wins, 0 if draw
    double scoreSum = 0.0;

public:
    MCTSNode(
        AstraDoBoard board,
        MCTSNode* parent,
        uint8_t move
        );
    ~MCTSNode();

    // Node Operations
    void expand();

    double random_rollout() const;

    void update(double score);


    // Getters
    const AstraDoBoard& getAstraDoBoard() const;

    MCTSNode* getParent() const;

    const std::vector<MCTSNode*>& getChildren() const;

    uint8_t getMove() const;

    bool isTerminal() const;

    double getAvgScore() const;

    double getAvgWinSum() const;

    int getNumVisits() const;

};

// Monte-Carlo Tree Search Algorithm
class MCTS{
private:
    // Root node of MCTS
    MCTSNode* root;
    int iterations;

    // Default minimum iterations before a node will be expanded
    const int DEFAULT_MIN_VISITS = 5;

    // Default hyperparameter c of MCTS (for balancing search depth and width)
    // Node that the value will be square-rooted in use
    const int DEFAULT_C = 2;

    double ucb(MCTSNode* node);

public:
    MCTS(
        AstraDoBoard initialBoard,
        int iterations = 10000
        );

    ~MCTS();

    // Function that runs the algorithm
    void run();

    // Procedure of MCTS
    // Select which node to visit in the current iteration
    MCTSNode* select(MCTSNode* node);

    // Expand the node that are visiting
    void expand(MCTSNode* node);

    // Perform rollouts
    double simulate(MCTSNode* node);

    // Update scores from bottom to top
    void backpropagate(MCTSNode* node, double score);

    // Interface to get the best move by MCTS in the current position
    uint8_t getBestMove();
};

#endif // MCTS_H
