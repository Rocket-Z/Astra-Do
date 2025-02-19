#include "mcts.h"

MCTSNode::MCTSNode(
    AstraDoBoard board,
    MCTSNode* parent,
    uint8_t move
    ) :
    board(board),
    parent(parent),
    move(move){

}

MCTSNode::~MCTSNode() {
    for(MCTSNode* child : children){
        delete child;
    }
}

void MCTSNode::expand(){
    if(board.getMoves().empty()){
        AstraDoBoard newState(board);
        newState.makeMove(100);
        children.push_back(new MCTSNode(newState, this, 100));
    }
    else{
        for(uint8_t move : board.getMoves()){
            AstraDoBoard newState(board);
            newState.makeMove(move);
            children.push_back(new MCTSNode(newState, this, move));
        }
    }
}

// Getters
const AstraDoBoard& MCTSNode::getAstraDoBoard() const { return board; }

MCTSNode* MCTSNode::getParent() const { return parent; }

const std::vector<MCTSNode*>& MCTSNode::getChildren() const { return children; }

uint8_t MCTSNode::getMove() const { return move; }

bool MCTSNode::isTerminal() const {
    // No legal moves can be made + previous move is stale
    return board.getMoves().size() == 0 && board.getStale();
}

double MCTSNode::getAvgScore() const { return scoreSum / numVisits; }

double MCTSNode::getAvgWinSum() const { return winSum / numVisits; }

int MCTSNode::getNumVisits() const { return numVisits; }

void MCTSNode::update(double score){
    this->scoreSum += score;
    if(score > 0){
        winSum += 1;
    }
    else if (score < 0){
        winSum -= 1;
    }
    ++numVisits;
}

double MCTSNode::random_rollout() const {

    AstraDoBoard rollout_board(board);

    while(true){
        // No legal moves can be made
        if(rollout_board.getMoves().size() == 0){
            // No potential moves for both sides, return piece count
            if(rollout_board.getStale()){
                std::pair<int, int> piece_count = rollout_board.getPieceCount();
                return piece_count.first - piece_count.second;
            }
            else{
                // Skip the move for the side
                rollout_board.makeMove(100);
            }
        }
        else{
            // Randomly plays a move
            rollout_board.makeMove(rollout_board.getMoves()[rand() % rollout_board.getMoves().size()]);
        }
    }

}


double MCTS::ucb(MCTSNode* node){
    if(node->getNumVisits() == 0) return std::numeric_limits<double>::infinity();
    // Reverse average score if turn is white
    return (node->getAstraDoBoard().getTurn() ? -node->getAvgWinSum() : node->getAvgWinSum()) +
           sqrt(DEFAULT_C * log(node->getParent()->getNumVisits()) / node->getNumVisits());
}

MCTS::MCTS(
    AstraDoBoard initialBoard,
    int iterations
    ) : iterations(iterations){
    root = new MCTSNode(initialBoard, nullptr, 100);
}


MCTS::~MCTS() {
    delete root;
}

void MCTS::run() {
    srand(time(0));
    for (int i = 0; i < iterations; ++i){
        MCTSNode* node = select(root);
        MCTSNode* rolloutNode;
        // Node is visited less than threshold, do no expand node
        // If the game has ended in the node, also do not expand
        if(node->getNumVisits() < DEFAULT_MIN_VISITS || node->isTerminal()){
            rolloutNode = node;
        }
        else{
            expand(node);
            rolloutNode = node->getChildren().empty() ? node : node->getChildren()[rand() % node->getChildren().size()];
        }
        double result = simulate(rolloutNode);
        backpropagate(rolloutNode, result);
    }
}

MCTSNode* MCTS::select(MCTSNode* node) {
    while(!node->getChildren().empty()) {
        node = *max_element(node->getChildren().begin(), node->getChildren().end(), [this](MCTSNode* a, MCTSNode* b) {
            return this->ucb(a) < this->ucb(b);
        });
    }
    return node;
}

void MCTS::expand(MCTSNode* node){
    node->expand();
}

double MCTS::simulate(MCTSNode* node){
    return node->random_rollout();
}

void MCTS::backpropagate(MCTSNode* node, double score){
    while (node) {
        node->update(score);
        node = node->getParent();
    }
}

uint8_t MCTS::getBestMove() {
    if(root->getAstraDoBoard().getMoves().empty()) return 54;
    else if(root->getAstraDoBoard().getMoves().size() == 1) return root->getAstraDoBoard().getMoves()[0];
    run();
    // printTree();
    MCTSNode* node = *std::max_element(root->getChildren().begin(), root->getChildren().end(), [this](MCTSNode* a, MCTSNode* b) {
        return this->ucb(a) < this->ucb(b);
    });
    // std::cout << "UCB" << std::endl;
    // for(MCTSNode* child: root->getChildren()){
    //     std::cout << static_cast<int>(child->getMove()) << ": " << ucb(child) << std::endl;
    // }
    return node->getMove();
}

