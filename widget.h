#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPoint>
#include <QVector>
#include <QKeyEvent>

// 障碍物结构体
struct Barrier
{
    int x;
    int topH;    // 上半部分高度
    int gap;     // 中间空隙高度
    int width;   // 障碍物宽度
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void gameLoop();    // 游戏主循环

private:
    QTimer *m_gameTimer;
    // 小鸟属性
    qreal m_birdY;
    qreal m_velocity;  // 垂直速度
    const qreal m_gravity; // 重力
    const qreal m_jumpForce; // 跳跃力度

    QVector<Barrier> m_barriers;
    int m_score;        // 分数
    bool m_gameOver;    // 游戏结束标记
    const int m_birdX;  // 小鸟固定X坐标
    int m_barrierSpeed; // 障碍物移动速度

    void initGame();    // 初始化游戏
    void createBarrier();// 生成障碍物
    bool checkCollision();// 碰撞检测
};

#endif // WIDGET_H
