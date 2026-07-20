#include "VisualPreviewWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <algorithm>

VisualPreviewWidget::VisualPreviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(320, 180);
    setMouseTracking(true);
}

void VisualPreviewWidget::setObsCanvasSize(const QSize& canvasSize)
{
    if (canvasSize.width() > 0 && canvasSize.height() > 0) {
        m_obsCanvasSize = canvasSize;
        update();
    }
}

void VisualPreviewWidget::setEffectItem(const EffectItem& item)
{
    m_currentEffect = item;
    update();
}

VisualPreviewWidget::BoundaryResult VisualPreviewWidget::checkBoundary(int centerX, int centerY, int sizePercent, const QSize& canvasSize) const
{
    BoundaryResult res;
    int W = canvasSize.width();
    int H = canvasSize.height();

    int elemW = (W * sizePercent) / 100;
    int elemH = (H * sizePercent) / 100;

    int minX = centerX - elemW / 2;
    int maxX = centerX + elemW / 2;
    int minY = centerY - elemH / 2;
    int maxY = centerY + elemH / 2;

    // 完全な画面外チェック
    if (maxX < 0 || minX > W || maxY < 0 || minY > H) {
        res.isOutOfBounds = true;
        res.warningMessage = "演出エリアが画面外にはみ出しています！";
    }

    // 自動クランピング補正
    int halfW = elemW / 2;
    int halfH = elemH / 2;
    res.clampedX = std::clamp(centerX, halfW, std::max(halfW, W - halfW));
    res.clampedY = std::clamp(centerY, halfH, std::max(halfH, H - halfH));

    return res;
}

QPoint VisualPreviewWidget::widgetToCanvas(const QPoint& pt) const
{
    double scaleX = static_cast<double>(m_obsCanvasSize.width()) / width();
    double scaleY = static_cast<double>(m_obsCanvasSize.height()) / height();
    return QPoint(qRound(pt.x() * scaleX), qRound(pt.y() * scaleY));
}

QPoint VisualPreviewWidget::canvasToWidget(const QPoint& pt) const
{
    double scaleX = static_cast<double>(width()) / m_obsCanvasSize.width();
    double scaleY = static_cast<double>(height()) / m_obsCanvasSize.height();
    return QPoint(qRound(pt.x() * scaleX), qRound(pt.y() * scaleY));
}

void VisualPreviewWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // キャンバス背景
    painter.fillRect(rect(), QColor(30, 30, 35));

    // 画面中央ガイド線
    painter.setPen(QPen(QColor(80, 80, 90), 1, Qt::DashLine));
    painter.drawLine(width() / 2, 0, width() / 2, height());
    painter.drawLine(0, height() / 2, width(), height() / 2);

    // キャンバス外枠
    painter.setPen(QPen(QColor(100, 100, 120), 2));
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // 演出アイテムの描画
    QPoint centerWidget = canvasToWidget(QPoint(m_currentEffect.centerX, m_currentEffect.centerY));

    int elemW = (width() * m_currentEffect.sizePercent) / 100;
    int elemH = (height() * m_currentEffect.sizePercent) / 100;

    QRect elemRect(centerWidget.x() - elemW / 2, centerWidget.y() - elemH / 2, elemW, elemH);

    // はみ出しチェック
    BoundaryResult bRes = checkBoundary(m_currentEffect.centerX, m_currentEffect.centerY, m_currentEffect.sizePercent, m_obsCanvasSize);

    QColor boxColor = bRes.isOutOfBounds ? QColor(220, 50, 50, 180) : QColor(50, 150, 250, 180);
    painter.fillRect(elemRect, boxColor);
    painter.setPen(QPen(bRes.isOutOfBounds ? Qt::red : Qt::cyan, 2));
    painter.drawRect(elemRect);

    // テキストプレビュー
    painter.setPen(Qt::white);
    painter.drawText(elemRect, Qt::AlignCenter, "演出プレビュー");

    // 警告メッセージ
    if (bRes.isOutOfBounds) {
        painter.setPen(Qt::yellow);
        painter.drawText(10, height() - 10, bRes.warningMessage);
    }
}

void VisualPreviewWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        QPoint canvasPt = widgetToCanvas(event->pos());
        emit positionChanged(canvasPt.x(), canvasPt.y());
    }
}

void VisualPreviewWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint canvasPt = widgetToCanvas(event->pos());
        emit positionChanged(canvasPt.x(), canvasPt.y());
    }
}

void VisualPreviewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
}
