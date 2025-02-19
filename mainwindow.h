#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "triangle.h"
#include "board.h"
#include "mcts.h"

#include <QString>

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QLabel>

#include <QHBoxLayout>
#include <QGridLayout>

#include <QCoreApplication>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    std::vector<std::shared_ptr<Triangle>> triangles;
    QPushButton *playAsWhiteButton;
    QPushButton *playAsBlackButton;
    QPushButton *playMyselfButton;
    QPushButton *restartButton;
    QPushButton *noLegalMoveButton;
    QLabel *blackPieceCountLabel;
    QLabel *whitePieceCountLabel;
    QLabel *currentTurnLabel;
    QLabel *notesLabel;

    AstraDoBoard board;
    enum class GameStatus { Init, PlayAsWhite, PlayAsBlack, PlayMyself, GameEnds };
    GameStatus gameStatus = GameStatus::Init;
    bool aiThinking = false;


    static const std::array<bool, 54> triangle_direction;
    static const std::vector<std::vector<int>> triangle_relative_pos;

    static constexpr double BORDER_PERCENTAGE = 0.12;
    static const int BORDER_WIDTH_HORIZONTAL = 0.866 * BORDER_PERCENTAGE * Triangle::TRIANGLE_LENGTH;
    static const int BORDER_WIDTH_VERTICAL = 1.5 * BORDER_PERCENTAGE * Triangle::TRIANGLE_LENGTH;

    static const int SCENE_WIDTH = 600;
    static const int SCENE_HEIGHT = 450;

    static const int MCTS_ITERS = 25000;

public:
    void restartGame();
    void playAsBlack();
    void playAsWhite();
    void playMyself();
    void onTriangleClicked(int moveID);
    void setTriangleStatus(uint8_t moveID);
    void makeAIMove();
    void skipMove();
    void updateLabels();

};
#endif // MAINWINDOW_H
