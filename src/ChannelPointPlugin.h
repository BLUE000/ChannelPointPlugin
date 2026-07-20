#ifndef CHANNEL_POINT_PLUGIN_H
#define CHANNEL_POINT_PLUGIN_H

#include <QObject>
#include "../tests/src/shared/plugin_interface.h"

class EffectManager;
class QueueManager;
class AnalyticsManager;
class ObsNotifier;
class PluginMainWidget;

class ChannelPointPlugin : public QObject, public IChannelPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.blue000.ChannelPointPlugin" FILE "plugin.json")
    Q_INTERFACES(IChannelPlugin)

public:
    ChannelPointPlugin();
    virtual ~ChannelPointPlugin() override;

    // IChannelPlugin インターフェース
    virtual void initialize(ICoreContext* context) override;
    virtual void shutdown() override;

    virtual QString pluginId() const override { return "com.blue000.channelpoint"; }
    virtual QString pluginName() const override { return "Channel Point Manager"; }
    virtual QString pluginVersion() const override { return "1.0.0"; }
    virtual QString pluginDescription() const override { return "Twitch チャンネルポイント演出制御・アナリティクスプラグイン"; }

    virtual QByteArray iconPngData() const override; // pic/ChannelPoint.png
    virtual QMap<QString, QByteArray> defaultAssets() const override;
    virtual QWidget* createWidget(QWidget* parent = nullptr) override;

    // イベント受信ハンドラ
    virtual void onCommentReceived(const TwitchComment& comment) override { Q_UNUSED(comment); }
    virtual void onRewardRedemptionReceived(const TwitchRewardRedemption& redemption) override;

private slots:
    void onTestPlayRequested(const QString& rewardId);
    void onEmergencyStopTriggered();

private:
    ICoreContext* m_context = nullptr;
    EffectManager* m_effectMgr = nullptr;
    ObsNotifier* m_obsNotifier = nullptr;
    QueueManager* m_queueMgr = nullptr;
    AnalyticsManager* m_analyticsMgr = nullptr;
    PluginMainWidget* m_mainWidget = nullptr;
};

#endif // CHANNEL_POINT_PLUGIN_H
