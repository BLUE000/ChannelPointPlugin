#ifndef OBS_NOTIFIER_H
#define OBS_NOTIFIER_H

#include <QObject>
#include <QJsonObject>
#include "../types/ChannelPointTypes.h"

class ICoreContext;

class ObsNotifier : public QObject {
    Q_OBJECT
public:
    explicit ObsNotifier(ICoreContext* context, QObject* parent = nullptr);

    // 演出表示リクエストの送出
    void sendRenderEffect(const QueueItem& queueItem, const EffectItem& effectItem, const QSize& canvasSize);

    // 演出消去リクエストの送出 (通常終了または緊急停止)
    void sendClearEffect(bool forceClear = false);

private:
    ICoreContext* m_context;
};

#endif // OBS_NOTIFIER_H
