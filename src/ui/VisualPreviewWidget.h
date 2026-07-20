#ifndef VISUAL_PREVIEW_WIDGET_H
#define VISUAL_PREVIEW_WIDGET_H

#include <QWidget>
#include <QSize>
#include <QPoint>
#include <QString>
#include "../types/ChannelPointTypes.h"

class VisualPreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit VisualPreviewWidget(QWidget* parent = nullptr);

    void setObsCanvasSize(const QSize& canvasSize);
    void setEffectItem(const EffectItem& item);

    struct BoundaryResult {
        bool isOutOfBounds = false;
        int clampedX = 0;
        int clampedY = 0;
        QString warningMessage;
    };

    // はみ出し判定・自動クランピング計算アルゴリズム
    BoundaryResult checkBoundary(int centerX, int centerY, int sizePercent, const QSize& canvasSize) const;

signals:
    void positionChanged(int newCenterX, int newCenterY);

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPoint widgetToCanvas(const QPoint& pt) const;
    QPoint canvasToWidget(const QPoint& pt) const;

    QSize m_obsCanvasSize{1920, 1080};
    EffectItem m_currentEffect;
    bool m_isDragging = false;
};

#endif // VISUAL_PREVIEW_WIDGET_H
