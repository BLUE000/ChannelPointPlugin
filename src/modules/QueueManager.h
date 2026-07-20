#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include "../types/ChannelPointTypes.h"

class EffectManager;
class ObsNotifier;

class QueueManager : public QObject {
    Q_OBJECT
public:
    explicit QueueManager(EffectManager* effectMgr, ObsNotifier* obsNotifier, QObject* parent = nullptr);

    void pushQueue(const QueueItem& item);
    void emergencyStop(); // 緊急停止 (キュー消去 + OBS即時クリア)

    int currentQueueCount() const { return m_queue.size(); }
    bool isPlaying() const { return m_isPlaying; }

signals:
    void queueCountChanged(int currentCount, int maxCount);
    void playbackStateChanged(bool isPlaying);

private slots:
    void processNext();
    void onPlaybackFinished();

private:
    EffectManager* m_effectMgr;
    ObsNotifier* m_obsNotifier;
    QQueue<QueueItem> m_queue;
    QTimer* m_playbackTimer;
    bool m_isPlaying = false;
};

#endif // QUEUE_MANAGER_H
