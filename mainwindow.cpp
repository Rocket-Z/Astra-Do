#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    scene(new QGraphicsScene(this)),
    view(new QGraphicsView(this)),
    board(AstraDoBoard())
{
    // Set up the main widget and layout
    QWidget *mainContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    scene->setBackgroundBrush(QColorConstants::Svg::brown);

    for(int i = 0; i < 54; ++i){
        std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>(i, triangle_direction[i]);
        triangles.push_back(triangle);
        scene->addItem(triangle.get());
        triangle->setPos(
            SCENE_WIDTH / 2 + triangle_relative_pos[i][0] * (Triangle::TRIANGLE_LENGTH / 2 + BORDER_WIDTH_HORIZONTAL)
                + BORDER_WIDTH_HORIZONTAL,
            SCENE_HEIGHT / 2 + triangle_relative_pos[i][1] * (Triangle::TRIANGLE_HEIGHT + BORDER_WIDTH_VERTICAL)
                + BORDER_WIDTH_VERTICAL / 3 * (triangle_direction[i] ? 2 : 1)
            );

        connect(triangle.get(), &Triangle::clicked, this, &MainWindow::onTriangleClicked);

    }
    // 100 indicates do not print legal moves
    setTriangleStatus(100);

    // **Create the button/label container**
    QWidget *menuContainer = new QWidget();
    QGridLayout *menuLayout = new QGridLayout(menuContainer);

    // Add buttons and labels to grid layout
    playAsWhiteButton = new QPushButton("Play as White");
    playAsBlackButton = new QPushButton("Play as Black");
    playMyselfButton = new QPushButton("Play Myself");
    restartButton = new QPushButton("Restart");
    noLegalMoveButton = new QPushButton("No Legal Moves");

    connect(playAsBlackButton, &QPushButton::clicked, this, &MainWindow::playAsBlack);
    connect(playAsWhiteButton, &QPushButton::clicked, this, &MainWindow::playAsWhite);
    connect(playMyselfButton, &QPushButton::clicked, this, &MainWindow::playMyself);
    connect(noLegalMoveButton, &QPushButton::clicked, this, &MainWindow::skipMove);
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::restartGame);

    blackPieceCountLabel = new QLabel("");
    whitePieceCountLabel = new QLabel("");
    currentTurnLabel = new QLabel("");
    notesLabel = new QLabel("Choose a mode to play!");

    menuLayout->addWidget(playAsBlackButton, 0, 0);
    menuLayout->addWidget(playAsWhiteButton, 0, 1);
    menuLayout->addWidget(playMyselfButton, 1, 0, 1, 2);  // Span across 2 columns
    menuLayout->addWidget(restartButton, 2, 0);
    menuLayout->addWidget(noLegalMoveButton, 2, 1);

    menuLayout->addWidget(blackPieceCountLabel, 3, 0, 1, 2);
    menuLayout->addWidget(whitePieceCountLabel, 4, 0, 1, 2);
    menuLayout->addWidget(currentTurnLabel, 5, 0, 1, 2);
    menuLayout->addWidget(notesLabel, 6, 0, 1, 2);

    menuContainer->setLayout(menuLayout);

    // Add components to layout
    mainLayout->addWidget(view);           // Triangle display (QGraphicsView)
    mainLayout->addWidget(menuContainer);

    mainContainer->setLayout(mainLayout);
    setCentralWidget(mainContainer);
}




MainWindow::~MainWindow()
{

}

void MainWindow::restartGame(){
    board = AstraDoBoard();
    gameStatus = GameStatus::Init;
    // 100 indicates do not print legal moves
    setTriangleStatus(100);
    updateLabels();
}

void MainWindow::playAsBlack(){
    gameStatus = GameStatus::PlayAsBlack;
    // AI plays the current move
    if(board.getTurn() == false){
        aiThinking = true;
        // 54 indicates game just started
        setTriangleStatus(54);
        updateLabels();
        // Force UI to update first
        QCoreApplication::processEvents();
        // AI moves first
        makeAIMove();
    }
    else{
        // 54 indicates game just started
        setTriangleStatus(54);
        updateLabels();
    }
}
void MainWindow::playAsWhite(){
    gameStatus = GameStatus::PlayAsWhite;
    // AI plays the current move
    if(board.getTurn() == true){
        aiThinking = true;
        // 54 indicates game just started
        setTriangleStatus(54);
        updateLabels();
        // Force UI to update first
        QCoreApplication::processEvents();
        // AI moves first
        makeAIMove();
    }
    else{
        // 54 indicates game just started
        setTriangleStatus(54);
        updateLabels();
    }
}

void MainWindow::playMyself(){
    gameStatus = GameStatus::PlayMyself;
    // 54 indicates game just started
    setTriangleStatus(54);
    updateLabels();
}

void MainWindow::onTriangleClicked(int moveID){
    switch(gameStatus){
    case GameStatus::Init:
    case GameStatus::GameEnds:
        break;
    case GameStatus::PlayAsBlack:
        // User's turn to play
        if(board.getTurn()){
            board.makeMove((uint8_t) moveID);
            // Game ends
            if(board.getMoves().size() == 0 && board.getStale()){
                gameStatus = GameStatus::GameEnds;
            }
            aiThinking = true;
            setTriangleStatus(moveID);
            updateLabels();
            // Force UI to update first
            QCoreApplication::processEvents();

            // Let MCTS make the next move
            makeAIMove();
        }
        break;
    case GameStatus::PlayAsWhite:
        // User's turn to play
        if(!board.getTurn()){
            board.makeMove((uint8_t) moveID);
            // Game ends
            if(board.getMoves().size() == 0 && board.getStale()){
                gameStatus = GameStatus::GameEnds;
            }
            aiThinking = true;
            setTriangleStatus(moveID);
            updateLabels();
            // Force UI to update first
            QCoreApplication::processEvents();

            // Let MCTS make the next move
            makeAIMove();
        }
        break;
    case GameStatus::PlayMyself:
        board.makeMove((uint8_t) moveID);
        // Game ends
        if(board.getMoves().size() == 0 && board.getStale()){
            gameStatus = GameStatus::GameEnds;
        }
        setTriangleStatus(moveID);
        updateLabels();
        break;
    default:
        break;
    }
}

void MainWindow::setTriangleStatus(uint8_t moveID){
    for(int i = 0; i < 54; ++i){
        if(board.getBlackPieces()[i]) triangles[i]->changeStatus(Triangle::TriangleStatus::BLACK);
        else if(board.getWhitePieces()[i]) triangles[i]->changeStatus(Triangle::TriangleStatus::WHITE);
        else triangles[i]->changeStatus(Triangle::TriangleStatus::EMPTY);
    }
    // Game not started, so do not print moves
    if(moveID > 54){
        return;
    }
    // Game just started, or previous move is skip
    else if(moveID == 54){
        for(uint8_t move : board.getMoves()){
            triangles[move]->changeStatus(Triangle::TriangleStatus::NEXT);
        }
    }
    else{
        for(uint8_t move : board.getMoves()){
            triangles[move]->changeStatus(Triangle::TriangleStatus::NEXT);
        }
        // Current turn is black, last turn is white
        if(board.getTurn()){
            triangles[moveID]->changeStatus(Triangle::TriangleStatus::WHITE_LAST);
        }
        // Current turn is white, last turn is black
        else{
            triangles[moveID]->changeStatus(Triangle::TriangleStatus::BLACK_LAST);
        }
    }
}

void MainWindow::makeAIMove(){
    // Let MCTS make the next move
    MCTS mcts(board, MCTS_ITERS);
    uint8_t move = mcts.getBestMove();
    board.makeMove(move);
    // Game ends
    if(board.getMoves().size() == 0 && board.getStale()){
        gameStatus = GameStatus::GameEnds;
    }
    aiThinking = false;
    setTriangleStatus(move);
    updateLabels();
}

void MainWindow::skipMove(){
    // Only active when there are no moves to be played
    // And game is still ongoing
    // And it's users' turn to play
    if(board.getMoves().size() == 0){
        if(
            (gameStatus == GameStatus::PlayAsBlack && board.getTurn())
        ||  (gameStatus == GameStatus::PlayAsWhite && !board.getTurn())
        ){
            board.makeMove(54);
            // Game ends
            if(board.getMoves().size() == 0 && board.getStale()){
                gameStatus = GameStatus::GameEnds;
            }
            aiThinking = true;
            setTriangleStatus(54);
            updateLabels();
            // Force UI to update first
            QCoreApplication::processEvents();

            // Let MCTS make the next move
            makeAIMove();
        }
        else if(gameStatus == GameStatus::PlayMyself){
            board.makeMove(54);
            // Game ends
            if(board.getMoves().size() == 0 && board.getStale()){
                gameStatus = GameStatus::GameEnds;
            }
            // 100 means do not print legal moves
            setTriangleStatus(54);
            updateLabels();
        }
    }
}

void MainWindow::updateLabels(){
    QString turn = board.getTurn() ? "Black" : "White";
    QString black_pieces_str = "Black piece count: ";
    QString white_pieces_str = "White piece count: ";
    int black_piece_count = board.getPieceCount().first;
    int white_piece_count = board.getPieceCount().second;
    QString game_end_str = "Game ends! ";
    switch(gameStatus){
    case GameStatus::Init:
        blackPieceCountLabel->setText("");
        whitePieceCountLabel->setText("");
        currentTurnLabel->setText("");
        notesLabel->setText("Choose a mode to play!");
        break;
    case GameStatus::PlayAsWhite:
        blackPieceCountLabel->setText(black_pieces_str.append(std::to_string(black_piece_count)));
        whitePieceCountLabel->setText(white_pieces_str.append(std::to_string(white_piece_count)));
        currentTurnLabel->setText(aiThinking ? "面瘫的黑脸男 is thinking..." : turn.append(" to play"));
        notesLabel->setText("Play against 面瘫的黑脸男 as white");
        break;
    case GameStatus::PlayAsBlack:
        blackPieceCountLabel->setText(black_pieces_str.append(std::to_string(black_piece_count)));
        whitePieceCountLabel->setText(white_pieces_str.append(std::to_string(white_piece_count)));
        currentTurnLabel->setText(aiThinking ? "面瘫的黑脸男  is thinking..." : turn.append(" to play"));
        notesLabel->setText("Play against 面瘫的黑脸男 as black");
        break;
    case GameStatus::PlayMyself:
        blackPieceCountLabel->setText(black_pieces_str.append(std::to_string(black_piece_count)));
        whitePieceCountLabel->setText(white_pieces_str.append(std::to_string(white_piece_count)));
        currentTurnLabel->setText(turn.append(" to play"));
        notesLabel->setText("");
        break;
    case GameStatus::GameEnds:
        blackPieceCountLabel->setText(black_pieces_str.append(std::to_string(black_piece_count)));
        whitePieceCountLabel->setText(white_pieces_str.append(std::to_string(white_piece_count)));
        if(black_piece_count > white_piece_count){
            currentTurnLabel->setText(game_end_str
                                        .append("Black wins by ")
                                        .append(std::to_string(black_piece_count - white_piece_count))
                                        .append(" pieces!")
                                    );
        }
        else if(black_piece_count < white_piece_count){
            currentTurnLabel->setText(game_end_str
                                          .append("White wins by ")
                                          .append(std::to_string(white_piece_count - black_piece_count))
                                          .append(" pieces!")
                                      );
        }
        else{
            currentTurnLabel->setText(game_end_str.append("Draw!"));
        }
        break;
    }
}

const std::array<bool, 54> MainWindow::triangle_direction = {
    1, 1, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 1, 0,
    1, 1, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 1, 0,
    1, 1, 0, 1, 1, 0, 1, 0, 1,
    0, 0, 1, 0, 0, 1, 0, 1, 0,
};

const std::vector<std::vector<int>> MainWindow::triangle_relative_pos = {
    {-1, 0}, {-2, 1}, {-1, 1}, {0, 1}, {-3, 2}, {-2, 2}, {-1, 2}, {0, 2}, {1, 2},
    {0, 0}, {1, 1}, {1, 0}, {2, 0}, {2, 2}, {2, 1}, {3, 1}, {3, 0}, {4, 0},
    {0, -1}, {2, -1}, {1, -1}, {1, -2}, {4, -1}, {3, -1}, {3, -2}, {2, -2}, {2, -3},
    {-1, -1}, {0, -2}, {-1, -2}, {-2, -2}, {1, -3}, {0, -3}, {-1, -3}, {-2, -3}, {-3, -3},
    {-2, -1}, {-3, -2}, {-3, -1}, {-4, -1}, {-4, -3}, {-4, -2}, {-5, -2}, {-5, -1}, {-6, -1},
    {-2, 0}, {-4, 0}, {-3, 0}, {-3, 1}, {-6, 0}, {-5, 0}, {-5, 1}, {-4, 1}, {-4, 2},
};
