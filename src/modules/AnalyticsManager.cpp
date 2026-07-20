#include "AnalyticsManager.h"
#include "EffectManager.h"
#include "../../tests/src/shared/plugin_interface.h"
#include <QDebug>
#include <QTextStream>

AnalyticsManager::AnalyticsManager(ICoreContext* context, EffectManager* effectMgr, QObject* parent)
    : QObject(parent)
    , m_context(context)
    , m_effectMgr(effectMgr)
{
    loadAnalytics();
}

bool AnalyticsManager::loadAnalytics()
{
    if (!m_context) return false;

    QByteArray data = m_context->readEncryptedFile("analytics_data.json");
    if (data.isEmpty()) {
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();
    m_sessionsMap.clear();

    QJsonArray sessionsArr = root.value("sessions").toArray();
    for (const QJsonValue& val : sessionsArr) {
        if (!val.isObject()) continue;
        QJsonObject sObj = val.toObject();
        StreamSession session;
        session.sessionId = sObj.value("session_id").toString();
        session.startTime = sObj.value("start_time").toVariant().toLongLong();
        session.endTime = sObj.value("end_time").toVariant().toLongLong();
        session.isActive = sObj.value("is_active").toBool();

        QJsonArray rewardArr = sObj.value("reward_stats").toArray();
        for (const QJsonValue& rVal : rewardArr) {
            QJsonObject rObj = rVal.toObject();
            RewardStat rStat;
            rStat.rewardId = rObj.value("reward_id").toString();
            rStat.rewardName = rObj.value("reward_name").toString();
            rStat.count = rObj.value("count").toInt();
            rStat.totalPoints = rObj.value("total_points").toInt();
            session.rewardStats.insert(rStat.rewardId, rStat);
        }

        QJsonArray userArr = sObj.value("user_stats").toArray();
        for (const QJsonValue& uVal : userArr) {
            QJsonObject uObj = uVal.toObject();
            UserStat uStat;
            uStat.userId = uObj.value("user_id").toString();
            uStat.username = uObj.value("username").toString();
            uStat.displayName = uObj.value("display_name").toString();
            uStat.count = uObj.value("count").toInt();
            uStat.totalPoints = uObj.value("total_points").toInt();
            session.userStats.insert(uStat.userId, uStat);
        }

        if (!session.sessionId.isEmpty()) {
            m_sessionsMap.insert(session.sessionId, session);
            if (session.isActive) {
                m_activeSessionId = session.sessionId;
            }
        }
    }

    emit analyticsUpdated();
    return true;
}

bool AnalyticsManager::saveAnalytics()
{
    if (!m_context) return false;

    QJsonObject root;
    QJsonArray sessionsArr;

    for (const StreamSession& session : m_sessionsMap.values()) {
        QJsonObject sObj;
        sObj["session_id"] = session.sessionId;
        sObj["start_time"] = session.startTime;
        sObj["end_time"] = session.endTime;
        sObj["is_active"] = session.isActive;

        QJsonArray rewardArr;
        for (const RewardStat& rStat : session.rewardStats.values()) {
            QJsonObject rObj;
            rObj["reward_id"] = rStat.rewardId;
            rObj["reward_name"] = rStat.rewardName;
            rObj["count"] = rStat.count;
            rObj["total_points"] = rStat.totalPoints;
            rewardArr.append(rObj);
        }
        sObj["reward_stats"] = rewardArr;

        QJsonArray userArr;
        for (const UserStat& uStat : session.userStats.values()) {
            QJsonObject uObj;
            uObj["user_id"] = uStat.userId;
            uObj["username"] = uStat.username;
            uObj["display_name"] = uStat.displayName;
            uObj["count"] = uStat.count;
            uObj["total_points"] = uStat.totalPoints;
            userArr.append(uObj);
        }
        sObj["user_stats"] = userArr;

        sessionsArr.append(sObj);
    }
    root["sessions"] = sessionsArr;

    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

    bool ok = m_context->writeEncryptedFile("analytics_data.json", jsonData);
    if (ok) {
        emit analyticsUpdated();
    }
    return ok;
}

void AnalyticsManager::recordRedemption(const QueueItem& item, int points)
{
    qint64 now = item.timestamp > 0 ? item.timestamp : QDateTime::currentSecsSinceEpoch();
    checkAndRotateSession(now);

    StreamSession& session = m_sessionsMap[m_activeSessionId];
    session.endTime = now;

    // 報酬統計更新
    RewardStat& rStat = session.rewardStats[item.rewardId];
    rStat.rewardId = item.rewardId;
    rStat.rewardName = item.rewardName;
    rStat.count++;
    rStat.totalPoints += points;

    // ユーザー統計更新
    QString uKey = item.userId.isEmpty() ? item.username : item.userId;
    UserStat& uStat = session.userStats[uKey];
    uStat.userId = item.userId;
    uStat.username = item.username;
    uStat.displayName = item.displayName;
    uStat.count++;
    uStat.totalPoints += points;

    m_lastEventTimestamp = now;
    saveAnalytics();
}

void AnalyticsManager::checkAndRotateSession(qint64 currentTimestamp)
{
    int gapMinutes = m_effectMgr ? m_effectMgr->getSessionGapMinutes() : 120;
    qint64 gapSecs = static_cast<qint64>(gapMinutes) * 60;

    bool needNewSession = false;

    if (m_activeSessionId.isEmpty() || !m_sessionsMap.contains(m_activeSessionId)) {
        needNewSession = true;
    } else if (m_lastEventTimestamp > 0 && (currentTimestamp - m_lastEventTimestamp) > gapSecs) {
        // 指定時間以上離れている場合は旧セッションを閉じ、新セッション作成
        m_sessionsMap[m_activeSessionId].isActive = false;
        m_sessionsMap[m_activeSessionId].endTime = m_lastEventTimestamp;
        needNewSession = true;
    }

    if (needNewSession) {
        QDateTime dt = QDateTime::fromSecsSinceEpoch(currentTimestamp);
        m_activeSessionId = "session_" + dt.toString("yyyyMMdd_hhmmss");

        StreamSession newSession;
        newSession.sessionId = m_activeSessionId;
        newSession.startTime = currentTimestamp;
        newSession.endTime = currentTimestamp;
        newSession.isActive = true;

        m_sessionsMap.insert(m_activeSessionId, newSession);
    }
}

QList<StreamSession> AnalyticsManager::getAllSessions() const
{
    return m_sessionsMap.values();
}

StreamSession AnalyticsManager::getSession(const QString& sessionId) const
{
    return m_sessionsMap.value(sessionId, StreamSession{});
}

StreamSession AnalyticsManager::getActiveSession() const
{
    return getSession(m_activeSessionId);
}

QString AnalyticsManager::exportSessionToCsv(const QString& sessionId) const
{
    StreamSession session = getSession(sessionId);
    if (session.sessionId.isEmpty()) return QString{};

    QString csv;
    QTextStream out(&csv);

    out << "Category,ID,Name,Count,TotalPoints\n";
    for (const RewardStat& r : session.rewardStats.values()) {
        out << "Reward," << r.rewardId << ",\"" << r.rewardName << "\"," << r.count << "," << r.totalPoints << "\n";
    }
    for (const UserStat& u : session.userStats.values()) {
        QString name = u.displayName.isEmpty() ? u.username : u.displayName;
        out << "User," << u.userId << ",\"" << name << "\"," << u.count << "," << u.totalPoints << "\n";
    }

    return csv;
}
