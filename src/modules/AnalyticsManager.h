#ifndef ANALYTICS_MANAGER_H
#define ANALYTICS_MANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include "../types/ChannelPointTypes.h"

class ICoreContext;
class EffectManager;

class AnalyticsManager : public QObject {
    Q_OBJECT
public:
    explicit AnalyticsManager(ICoreContext* context, EffectManager* effectMgr, QObject* parent = nullptr);

    bool loadAnalytics();
    bool saveAnalytics();

    // イベント記録 (自動セッション切り替え判定を含む)
    void recordRedemption(const QueueItem& item, int points);

    QList<StreamSession> getAllSessions() const;
    StreamSession getSession(const QString& sessionId) const;
    StreamSession getActiveSession() const;

    // CSV エクスポート
    QString exportSessionToCsv(const QString& sessionId) const;

signals:
    void analyticsUpdated();

private:
    void checkAndRotateSession(qint64 currentTimestamp);

    ICoreContext* m_context;
    EffectManager* m_effectMgr;
    QMap<QString, StreamSession> m_sessionsMap;
    QString m_activeSessionId;
    qint64 m_lastEventTimestamp = 0;
};

#endif // ANALYTICS_MANAGER_H
