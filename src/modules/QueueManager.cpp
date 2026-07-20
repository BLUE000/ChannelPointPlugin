#include "QueueManager.h"
#include "EffectManager.h"
#include "ObsNotifier.h"
#include <QDebug>

QueueManager::QueueManager(EffectManager* effectMgr, ObsNotifier* obsNotifier, QObject* parent)
    : QObject(parent)
    , m_effectMgr(effectMgr)
    , m_obsNotifier(obsNotifier)
{
    m_playbackTimer = new QTimer(this);
    m_playbackTimer->setSingleShot(true);
    connect(m_playbackTimer, &QTimer::timeout, this, &QueueManager::onPlaybackFinished);
}

void QueueManager::pushQueue(const QueueItem& item)
{
    if (!m_effectMgr) return;

    int maxQueue = m_effectMgr->getMaxQueueSize();
    QueueOverflowPolicy policy = m_effectMgr->getOverflowPolicy();

    // キューサイズチェック
    if (m_queue.size() >= maxQueue) {
        if (policy == QueueOverflowPolicy::DropOldest) {
            if (!m_queue.isEmpty()) {
                m_queue.dequeue(); // 最古のキューをドロップ
            }
            m_queue.enqueue(item);
        } else {
            // RejectNewest: 新しいキューを破棄
            qDebug() << "Queue full. Rejecting new redemption:" << item.redemptionId;
            return;
        }
    } else {
        m_queue.enqueue(item);
    }

    emit queueCountChanged(m_queue.size(), maxQueue);

    if (!m_isPlaying) {
        processNext();
    }
}

void QueueManager::emergencyStop()
{
    // タイマー停止
    m_playbackTimer->stop();
    m_isPlaying = false;

    // キュー全消去
    m_queue.clear();

    int maxQueue = m_effectMgr ? m_effectMgr->getMaxQueueSize() : 20;
    emit queueCountChanged(0, maxQueue);
    emit playbackStateChanged(false);

    // OBSへ即時消去命令を優先送信
    if (m_obsNotifier) {
        m_obsNotifier->sendClearEffect(true);
    }
}

void QueueManager::processNext()
{
    if (m_queue.isEmpty()) {
        m_isPlaying = false;
        emit playbackStateChanged(false);
        return;
    }

    m_isPlaying = true;
    emit playbackStateChanged(true);

    QueueItem item = m_queue.dequeue();
    int maxQueue = m_effectMgr ? m_effectMgr->getMaxQueueSize() : 20;
    emit queueCountChanged(m_queue.size(), maxQueue);

    // 報酬設定および演出マスターから設定を取得
    RewardEffectSetting setting = m_effectMgr->getSetting(item.rewardId);
    EffectItem effectItem;
    if (!setting.effects.isEmpty()) {
        // 対象の演出IDまたは最初の演出
        effectItem = setting.effects.first();
        for (const auto& eff : setting.effects) {
            if (eff.effectId == item.targetEffectId) {
                effectItem = eff;
                break;
            }
        }
    } else {
        effectItem = m_effectMgr->getDefaultEffect();
    }

    QSize canvasSize = m_effectMgr->getObsCanvasSize();

    // OBSへ演出送出
    if (m_obsNotifier) {
        m_obsNotifier->sendRenderEffect(item, effectItem, canvasSize);
    }

    // 表示秒数タイマーセット (最小1秒)
    int durationMs = qMax(1, effectItem.durationSec) * 1000;
    m_playbackTimer->start(durationMs);
}

void QueueManager::onPlaybackFinished()
{
    // 演出終了通知
    if (m_obsNotifier) {
        m_obsNotifier->sendClearEffect(false);
    }

    // 次のキューを処理
    processNext();
}
