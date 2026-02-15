#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    int buttonX = 20;
    int buttonWidth = 200;
    int buttonHeight = 30;
    int buttonY = 450;
    int spacing = 10;

    loadButton = new QPushButton("Load maze", this);
    loadButton->setGeometry(buttonX, buttonY, buttonWidth, buttonHeight);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);

    computeButton = new QPushButton("Load BFS", this);
    computeButton->setGeometry(buttonX, buttonY + buttonHeight + spacing, buttonWidth, buttonHeight);
    computeButton->setEnabled(false);
    connect(computeButton, &QPushButton::clicked, this, &MainWindow::onComputePathsClicked);

    resize(1000, 600);
}

MainWindow::~MainWindow() {}


void MainWindow::onLoadFileClicked() {
    QString filePath = QFileDialog::getOpenFileName(this,
        "Choose maze file",
        QDir::homePath(),
        "Text files (*.txt);; (*)");

    if (!filePath.isEmpty()) {
        loadLabyrinthFromFile(filePath);
    }
}


void MainWindow::loadLabyrinthFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "File reading error:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    std::string labirintData;
    while (!in.atEnd()) {
        labirintData += in.readLine().toStdString() + "\n";
    }

    resetVisualization();
    g.readFromString(labirintData);

    if (g.matrix.empty()) {
        computeButton->setEnabled(false);
    }
    else {
        g.buildGraph();
        computeButton->setEnabled(true);
        update();
    }
}


void MainWindow::resetVisualization() {
    showPaths = false;
    pathMask.clear();
    computeButton->setText("Load BFS");
}


void MainWindow::onComputePathsClicked() {
    if (showPaths) {
        resetVisualization();
    }
    else {
        g.bfs();
        computePaths();
        showPaths = true;
        computeButton->setText("Hide paths");
    }
    update();
}


void MainWindow::computePaths() {
    int n = g.matrix.size();
    int m = g.matrix[0].size();
    pathMask.assign(n, std::vector<bool>(m, false));

    for (auto ex : g.exits) {
        auto path = g.getPathTo(ex);
        for (auto node : path)
            pathMask[node.x][node.y] = true;
    }
}

void MainWindow::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);

    int cellSize = 60;

    drawLabyrinth(p, cellSize);

    int offsetX = width() / 2 + 30;
    drawGraph(p, offsetX, cellSize);
}

void MainWindow::drawLabyrinth(QPainter& p, int cellSize) {
    int offsetY = 50;
    for (int i = 0; i < (int)g.matrix.size(); i++) {
        for (int j = 0; j < (int)g.matrix[i].size(); j++) {
            QRect rect(j * cellSize + 20, i * cellSize + offsetY, cellSize, cellSize);

            if (g.matrix[i][j] == 0)
                p.fillRect(rect, Qt::black);
            else if (g.matrix[i][j] == 3)
                p.fillRect(rect, Qt::cyan);
            else if (g.matrix[i][j] == 2)
                p.fillRect(rect, Qt::red);
            else if (showPaths && pathMask[i][j])
                p.fillRect(rect, Qt::green);
            else
                p.fillRect(rect, Qt::white);

            p.drawRect(rect);
        }
    }
}


void MainWindow::drawGraph(QPainter& p, int offsetX, int cellSize) {
    int offsetY = 50;
    std::map<Node, QPoint> nodePos;
    std::map<Node, int> nodeIndex;

    int index = 1;
    for (auto& kv : g.adj) {
        Node n = kv.first;
        nodeIndex[n] = index++;

        int x_pos = offsetX + n.y * cellSize + cellSize / 2;
        int y_pos = offsetY + n.x * cellSize + cellSize / 2;
        nodePos[n] = QPoint(x_pos, y_pos);
    }


    p.setPen(Qt::gray);
    for (auto& kv : g.adj) {
        QPoint a = nodePos[kv.first];
        for (auto& neigh : kv.second) {
            if (nodePos.find(neigh) != nodePos.end())
                p.drawLine(a, nodePos[neigh]);
        }
    }


    int nodeRadius = 15;
    for (auto& kv : nodePos) {
        Node n = kv.first;
        QPoint pt = kv.second;

        QColor color = Qt::white;
        if (g.matrix[n.x][n.y] == 3)
            color = Qt::cyan;
        else if (g.matrix[n.x][n.y] == 2)
            color = Qt::red;
        else if (showPaths && pathMask[n.x][n.y])
            color = Qt::green;

        p.setBrush(color);
        p.setPen(Qt::black);
        p.drawEllipse(pt, nodeRadius, nodeRadius);
    }


    p.setPen(Qt::black);
    QFont font = p.font();
    font.setPointSize(10);
    p.setFont(font);


    for (auto& kv : nodePos) {
        Node n = kv.first;
        QPoint pt = kv.second;

        QString nodeLabel = QString("%1").arg(nodeIndex[n]);

        QRect textRect(pt.x() - nodeRadius * 2, pt.y() - nodeRadius * 2,
            nodeRadius * 4, nodeRadius * 4);

        p.drawText(textRect, Qt::AlignCenter, nodeLabel);
    }
}
