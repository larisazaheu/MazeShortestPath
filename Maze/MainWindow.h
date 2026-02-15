#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QFileDialog>

#include "Graph.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onComputePathsClicked();
    void onLoadFileClicked();

private:
    Graph g;
    bool showPaths = false;
    std::vector<std::vector<bool>> pathMask;
    QPushButton* computeButton;
    QPushButton* loadButton;

    void computePaths();
    void drawLabyrinth(QPainter& p, int cellSize);
    void drawGraph(QPainter& p, int offsetX, int cellSize);

    void loadLabyrinthFromFile(const QString& filePath);
    void resetVisualization();
};
