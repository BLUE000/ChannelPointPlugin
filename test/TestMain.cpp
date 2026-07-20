#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include "../src/types/ChannelPointTypes.h"
#include "../src/modules/EffectManager.h"
#include "../src/modules/QueueManager.h"
#include "../src/modules/ObsNotifier.h"
#include "../src/modules/AnalyticsManager.h"
#include "../src/ui/VisualPreviewWidget.h"
#include "../tests/src/shared/plugin_interface.h"

// モック ICoreContext
class MockCoreContext : public ICoreContext {
public:
    virtual void sendChatMessage(const QString& message) override { Q_UNUSED(message); }
    virtual void requestTts(const QString& text, const QString& speakerId, int speed, int pitch, int volume) override { Q_UNUSED(text); Q_UNUSED(speakerId); Q_UNUSED(speed); Q_UNUSED(pitch); Q_UNUSED(volume); }
    virtual void sendToObs(const QString& action, const QJsonObject& payload) override {
        lastAction = action;
        lastPayload = payload;
        sendCount++;
    }
    virtual void postDiscordWebhook(const QString& webhookUrl, const QJsonObject& payload) override { Q_UNUSED(webhookUrl); Q_UNUSED(payload); }
    virtual QList<TwitchRewardInfo> getChannelPointRewards() override { return mockRewards; }
    virtual QString getPluginDirectory() const override { return "."; }
    virtual QString getCipherKey() const override { return "test_cipher_key"; }
    virtual bool writeEncryptedFile(const QString& relativePath, const QByteArray& data) override {
        storage[relativePath] = data;
        return true;
    }
    virtual QByteArray readEncryptedFile(const QString& relativePath) override {
        return storage.value(relativePath, QByteArray());
    }
    virtual void writeLog(const QString& level, const QString& className, const QString& funcName, const QString& description) override { Q_UNUSED(level); Q_UNUSED(className); Q_UNUSED(funcName); Q_UNUSED(description); }

    void clearSpy() {
        lastAction.clear();
        lastPayload = QJsonObject{};
        sendCount = 0;
    }

    QString lastAction;
    QJsonObject lastPayload;
    int sendCount = 0;
    QList<TwitchRewardInfo> mockRewards;
    QMap<QString, QByteArray> storage;
};

class PluginAutoTestSuite : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // 単体テスト (Unit Tests: UT)
    void testEffectManager_data();
    void testEffectManager();
    void testQueueManager_overflowAndEmergencyStop();
    void testVisualPreviewWidget_boundaryCheck();
    void testAnalyticsManager_autoSessionRotation();

    // 結合テスト (Integration Tests: IT)
    void testIntegration_eventToObsPipeline();
    void testIntegration_emergencyStopPipeline();

    // システムテスト (System Tests: ST)
    void testSystem_spam100EventsEndurance();
    void testSystem_dataPersistenceAndRestore();

private:
    MockCoreContext* m_mockContext = nullptr;
};

void PluginAutoTestSuite::init()
{
    m_mockContext = new MockCoreContext();
}

void PluginAutoTestSuite::cleanup()
{
    delete m_mockContext;
    m_mockContext = nullptr;
}

void PluginAutoTestSuite::testEffectManager_data()
{
}

void PluginAutoTestSuite::testEffectManager()
{
    EffectManager effMgr(m_mockContext);
    
    RewardEffectSetting setting;
    setting.rewardId = "test_reward_1";
    setting.rewardName = "テスト報酬1";
    setting.points = 500;
    
    EffectItem item;
    item.effectId = "eff_1";
    item.durationSec = 10;
    setting.effects.append(item);

    effMgr.updateSetting(setting);

    // ロード検証
    RewardEffectSetting retrieved = effMgr.getSetting("test_reward_1");
    QCOMPARE(retrieved.rewardName, QString("テスト報酬1"));
    QCOMPARE(retrieved.points, 500);
    QCOMPARE(retrieved.effects.size(), 1);
    QCOMPARE(retrieved.effects.first().durationSec, 10);
}

void PluginAutoTestSuite::testQueueManager_overflowAndEmergencyStop()
{
    EffectManager effMgr(m_mockContext);
    effMgr.setMaxQueueSize(5); // キュー上限 5件
    effMgr.setOverflowPolicy(QueueOverflowPolicy::DropOldest);

    ObsNotifier obsNotifier(m_mockContext);
    QueueManager queueMgr(&effMgr, &obsNotifier);

    // 10件投入 (オーバーフロー発生)
    for (int i = 1; i <= 10; ++i) {
        QueueItem item;
        item.redemptionId = QString("redem_%1").arg(i);
        item.rewardId = "test_reward";
        queueMgr.pushQueue(item);
    }

    // ドロップされてサイズが最大5件以下であること
    QVERIFY(queueMgr.currentQueueCount() <= 5);

    // 緊急停止テスト
    queueMgr.emergencyStop();
    QCOMPARE(queueMgr.currentQueueCount(), 0);
    QCOMPARE(m_mockContext->lastAction, QString("channel_point_effect"));
    QCOMPARE(m_mockContext->lastPayload.value("action").toString(), QString("clear_effect"));
    QCOMPARE(m_mockContext->lastPayload.value("force").toBool(), true);
}

void PluginAutoTestSuite::testVisualPreviewWidget_boundaryCheck()
{
    VisualPreviewWidget widget;
    QSize canvas(1920, 1080);

    // 画面中央 (1920x1080 の範囲内)
    auto res1 = widget.checkBoundary(960, 540, 60, canvas);
    QCOMPARE(res1.isOutOfBounds, false);
    QCOMPARE(res1.clampedX, 960);
    QCOMPARE(res1.clampedY, 540);

    // 範囲外 (マイナス座標) -> クランピング補正
    auto res2 = widget.checkBoundary(-500, -500, 60, canvas);
    QCOMPARE(res2.isOutOfBounds, true);
    QCOMPARE(res2.clampedX, 576); // halfW = (1920*60%/2) = 576
    QCOMPARE(res2.clampedY, 324); // halfH = (1080*60%/2) = 324
}

void PluginAutoTestSuite::testAnalyticsManager_autoSessionRotation()
{
    EffectManager effMgr(m_mockContext);
    effMgr.setSessionGapMinutes(120); // 2時間

    AnalyticsManager analytics(m_mockContext, &effMgr);

    qint64 t1 = 1000000;
    QueueItem item1;
    item1.redemptionId = "r1";
    item1.rewardId = "rew_1";
    item1.username = "Alice";
    item1.timestamp = t1;

    analytics.recordRedemption(item1, 100);
    QCOMPARE(analytics.getAllSessions().size(), 1);

    // 3時間後 (10800秒) のイベント
    qint64 t2 = t1 + 10800;
    QueueItem item2;
    item2.redemptionId = "r2";
    item2.rewardId = "rew_1";
    item2.username = "Bob";
    item2.timestamp = t2;

    analytics.recordRedemption(item2, 200);

    // 自動で新セッションが作成され、計2件のセッションになること
    QCOMPARE(analytics.getAllSessions().size(), 2);
}

void PluginAutoTestSuite::testIntegration_eventToObsPipeline()
{
    EffectManager effMgr(m_mockContext);
    ObsNotifier obsNotifier(m_mockContext);
    QueueManager queueMgr(&effMgr, &obsNotifier);

    QueueItem item;
    item.redemptionId = "integration_1";
    item.rewardId = "rew_int";
    item.rewardName = "水";
    item.username = "Charlie";
    item.displayName = "チャーリー";

    queueMgr.pushQueue(item);

    // OBSへ送信されたペイロードのアサート検証
    QCOMPARE(m_mockContext->sendCount, 1);
    QCOMPARE(m_mockContext->lastAction, QString("channel_point_effect"));
    QCOMPARE(m_mockContext->lastPayload.value("action").toString(), QString("render_effect"));
    QCOMPARE(m_mockContext->lastPayload.value("display_name").toString(), QString("チャーリー"));
}

void PluginAutoTestSuite::testIntegration_emergencyStopPipeline()
{
    EffectManager effMgr(m_mockContext);
    ObsNotifier obsNotifier(m_mockContext);
    QueueManager queueMgr(&effMgr, &obsNotifier);

    m_mockContext->clearSpy();

    queueMgr.emergencyStop();
    QCOMPARE(m_mockContext->sendCount, 1);
    QCOMPARE(m_mockContext->lastPayload.value("action").toString(), QString("clear_effect"));
}

void PluginAutoTestSuite::testSystem_spam100EventsEndurance()
{
    EffectManager effMgr(m_mockContext);
    effMgr.setMaxQueueSize(20);
    ObsNotifier obsNotifier(m_mockContext);
    QueueManager queueMgr(&effMgr, &obsNotifier);
    AnalyticsManager analytics(m_mockContext, &effMgr);

    // 100件スパム投入
    for (int i = 0; i < 100; ++i) {
        QueueItem item;
        item.redemptionId = QString("spam_%1").arg(i);
        item.rewardId = "rew_spam";
        item.rewardName = "スパム";
        item.username = "Spammer";

        analytics.recordRedemption(item, 10);
        queueMgr.pushQueue(item);
    }

    // アナリティクスには100件漏れなく記録されていること
    StreamSession activeSession = analytics.getActiveSession();
    QCOMPARE(activeSession.userStats["Spammer"].count, 100);

    // キューサイズは上限20件に抑制されていること
    QVERIFY(queueMgr.currentQueueCount() <= 20);
}

void PluginAutoTestSuite::testSystem_dataPersistenceAndRestore()
{
    {
        EffectManager effMgr(m_mockContext);
        RewardEffectSetting s;
        s.rewardId = "persist_rew";
        s.rewardName = "永続化テスト";
        effMgr.updateSetting(s);
    }

    // 別インスタンスで復元読み込み
    EffectManager effMgr2(m_mockContext);
    RewardEffectSetting s2 = effMgr2.getSetting("persist_rew");
    QCOMPARE(s2.rewardName, QString("永続化テスト"));
}

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PluginAutoTestSuite tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "TestMain.moc"
