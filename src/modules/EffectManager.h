#ifndef EFFECT_MANAGER_H
#define EFFECT_MANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QSize>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "../types/ChannelPointTypes.h"

// 前方宣言
class ICoreContext;

class EffectManager : public QObject {
    Q_OBJECT
public:
    explicit EffectManager(ICoreContext* context, QObject* parent = nullptr);

    bool loadSettings();
    bool saveSettings();

    QList<RewardEffectSetting> getAllSettings() const;
    RewardEffectSetting getSetting(const QString& rewardId) const;
    void updateSetting(const RewardEffectSetting& setting);
    void removeSetting(const QString& rewardId);
    void ensureRewardRegistered(const QString& rewardId, const QString& rewardName, int points = 0);

    // OBS 解像度設定
    QSize getObsCanvasSize() const { return m_obsCanvasSize; }
    void setObsCanvasSize(const QSize& size);

    // キュー設定
    int getMaxQueueSize() const { return m_maxQueueSize; }
    void setMaxQueueSize(int size);

    QueueOverflowPolicy getOverflowPolicy() const { return m_overflowPolicy; }
    void setOverflowPolicy(QueueOverflowPolicy policy);

    // セッション判定ギャップ時間 (分)
    int getSessionGapMinutes() const { return m_sessionGapMinutes; }
    void setSessionGapMinutes(int minutes);

    // デフォルト演出設定の取得
    EffectItem getDefaultEffect() const;

signals:
    void settingsUpdated();

private:
    ICoreContext* m_context;
    QSize m_obsCanvasSize{1920, 1080};
    int m_maxQueueSize = 20;
    QueueOverflowPolicy m_overflowPolicy = QueueOverflowPolicy::DropOldest;
    int m_sessionGapMinutes = 120;

    QMap<QString, RewardEffectSetting> m_settingsMap;
};

#endif // EFFECT_MANAGER_H
