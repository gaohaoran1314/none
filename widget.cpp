#include "widget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QFont>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    m_gravity(0.35),
    m_jumpForce(-8.5),
    m_birdX(120),
    m_barrierSpeed(3)
{
    setWindowTitle("笨鸟先飞 - 跳跃障碍");
    resize(600, 450);

    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &Widget::gameLoop);
    m_gameTimer->start(16); // 约60帧

    initGame();
}

Widget::~Widget()
{

}

// 初始化游戏状态
void Widget::initGame()
{
    m_birdY = height() / 2.0;
    m_velocity = 0;
    m_score = 0;
    m_gameOver = false;
    m_barriers.clear();
}

// 随机生成一组障碍物
void Widget::createBarrier()
{
    Barrier b;
    b.x = width();
    b.width = 50;
    b.gap = 120; // 通道高度
    // 随机上障碍物高度
    b.topH = QRandomGenerator::global()->bounded(80, height() - b.gap - 80);
    m_barriers.append(b);
}

// 碰撞检测
bool Widget::checkCollision()
{
    int birdW = 30;
    int birdH = 25;
    // 撞上下边界
    if(m_birdY <= 0 || m_birdY + birdH >= height())
        return true;

    // 遍历障碍物
    for(auto &bar : m_barriers)
    {
        // X范围相交
        if(m_birdX + birdW > bar.x && m_birdX < bar.x + bar.width)
        {
            // 撞上上方障碍 或 下方障碍
            if(m_birdY < bar.topH || m_birdY + birdH > bar.topH + bar.gap)
            {
                return true;
            }
        }
    }
    return false;
}

// 游戏主循环
void Widget::gameLoop()
{
    if(m_gameOver) return;

    // 重力下落
    m_velocity += m_gravity;
    m_birdY += m_velocity;

    // 移动障碍物
    for(auto &bar : m_barriers)
    {
        bar.x -= m_barrierSpeed;
    }

    // 移除移出屏幕的障碍
    while(!m_barriers.empty() && m_barriers.first().x + m_barriers.first().width < 0)
    {
        m_barriers.removeFirst();
    }

    // 定时生成新障碍
    if(m_barriers.isEmpty() || m_barriers.last().x < width() - 220)
    {
        createBarrier();
    }

    // 计分：穿过障碍加分
    for(auto &bar : m_barriers)
    {
        if(bar.x + bar.width == m_birdX)
        {
            m_score++;
        }
    }

    // 碰撞判定
    if(checkCollision())
    {
        m_gameOver = true;
    }

    update();
}

// 键盘按下：空格跳跃
void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        if(m_gameOver)
        {
            initGame(); // 游戏结束按空格重新开始
        }
        else
        {
            m_velocity = m_jumpForce; // 施加跳跃力
        }
    }
    QWidget::keyPressEvent(event);
}

// 鼠标点击跳跃
void Widget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(m_gameOver)
    {
        initGame();
    }
    else
    {
        m_velocity = m_jumpForce;
    }
    QWidget::mousePressEvent(event);
}

// 绘制所有画面
void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = width();
    int h = height();

    // 1. 背景
    painter.fillRect(0, 0, w, h, QColor(140, 210, 255));

    // 2. 绘制障碍物
    painter.setBrush(QColor(34, 139, 34));
    painter.setPen(Qt::black);
    for(auto &bar : m_barriers)
    {
        // 上障碍物
        painter.drawRect(bar.x, 0, bar.width, bar.topH);
        // 下障碍物
        painter.drawRect(bar.x, bar.topH + bar.gap, bar.width, h - bar.topH - bar.gap);
    }

    // 3. 绘制小鸟
    painter.setBrush(QColor(255, 150, 0));
    painter.drawEllipse(m_birdX, m_birdY, 30, 25);
    // 鸟嘴
    painter.setBrush(Qt::red);
    painter.drawRect(m_birdX + 28, m_birdY + 8, 8, 4);

    // 4. 绘制分数
    painter.setPen(Qt::white);
    QFont f("微软雅黑", 20, QFont::Bold);
    painter.setFont(f);
    painter.drawText(20, 35, QString("分数：%1").arg(m_score));

    // 5. 游戏结束提示
    if(m_gameOver)
    {
        painter.setPen(Qt::red);
        QFont endFont("微软雅黑", 28, QFont::Bold);
        painter.setFont(endFont);
        painter.drawText(rect(), Qt::AlignCenter, "游戏结束\n点击/按空格重新开始");
    }
}
