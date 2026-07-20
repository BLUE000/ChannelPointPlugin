#include "ChannelPointPlugin.h"
#include "modules/EffectManager.h"
#include "modules/ObsNotifier.h"
#include "modules/QueueManager.h"
#include "modules/AnalyticsManager.h"
#include "ui/PluginMainWidget.h"

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

ChannelPointPlugin::ChannelPointPlugin()
{
}

ChannelPointPlugin::~ChannelPointPlugin()
{
    shutdown();
}

void ChannelPointPlugin::initialize(ICoreContext* context)
{
    m_context = context;
    if (!m_context) return;

    m_effectMgr = new EffectManager(m_context, this);
    m_obsNotifier = new ObsNotifier(m_context, this);
    m_queueMgr = new QueueManager(m_effectMgr, m_obsNotifier, this);
    m_analyticsMgr = new AnalyticsManager(m_context, m_effectMgr, this);
}

void ChannelPointPlugin::shutdown()
{
    if (m_queueMgr) {
        m_queueMgr->emergencyStop();
    }
}

QByteArray ChannelPointPlugin::iconPngData() const
{
    // pic/ChannelPoint.png を返却
    QFile file("pic/ChannelPoint.png");
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    return QByteArray();
}

QMap<QString, QByteArray> ChannelPointPlugin::defaultAssets() const
{
    QMap<QString, QByteArray> assets;

    // overlay.html
    QByteArray html = R"(<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="UTF-8">
    <title>Channel Point Overlay</title>
    <link rel="stylesheet" href="overlay.css">
</head>
<body>
    <div id="container"></div>
    <script src="overlay.js"></script>
</body>
</html>)";

    // overlay.js (IP/ポートは動的に解決)
    QByteArray js = R"(
(function() {
    let ws = null;
    function connect() {
        // location.hostname から動的にサーバーアドレスを判別 (2PC/LAN対応)
        let host = location.hostname || 'localhost';
        let port = location.port || '8080';
        let wsUrl = 'ws://' + host + ':' + port + '/ws';
        
        ws = new WebSocket(wsUrl);
        ws.onmessage = function(event) {
            try {
                let data = JSON.parse(event.data);
                if (data.action === 'render_effect') {
                    renderEffect(data);
                } else if (data.action === 'clear_effect') {
                    clearEffect(data.force);
                }
            } catch (e) {
                console.error(e);
            }
        };
        ws.onclose = function() {
            setTimeout(connect, 3000);
        };
    }

    function renderEffect(data) {
        let container = document.getElementById('container');
        container.innerHTML = '';

        let eff = data.effect;
        let elem = document.createElement('div');
        elem.className = 'effect-item pop-in';
        elem.style.left = eff.center_x + 'px';
        elem.style.top = eff.center_y + 'px';

        if (eff.image_path) {
            let img = document.createElement('img');
            img.src = eff.image_path;
            elem.appendChild(img);
        }

        if (eff.display_text) {
            let txt = document.createElement('div');
            txt.className = 'effect-text';
            txt.innerText = eff.display_text;
            elem.appendChild(txt);
        }

        if (eff.audio_path) {
            let audio = new Audio(eff.audio_path);
            audio.play();
        }

        container.appendChild(elem);
    }

    function clearEffect(force) {
        let container = document.getElementById('container');
        if (container) {
            container.innerHTML = '';
        }
    }

    connect();
})();
)";

    // overlay.css
    QByteArray css = R"(
body {
    margin: 0;
    padding: 0;
    overflow: hidden;
    background-color: transparent;
}
#container {
    position: relative;
    width: 100vw;
    height: 100vh;
}
.effect-item {
    position: absolute;
    transform: translate(-50%, -50%);
    text-align: center;
}
.effect-text {
    font-size: 28px;
    font-weight: bold;
    color: white;
    text-shadow: 2px 2px 4px black;
}
.pop-in {
    animation: popIn 0.4s ease-out forwards;
}
@keyframes popIn {
    0% { transform: translate(-50%, -50%) scale(0.2); opacity: 0; }
    100% { transform: translate(-50%, -50%) scale(1.0); opacity: 1; }
}
)";

    assets.insert("overlay.html", html);
    assets.insert("overlay.js", js);
    assets.insert("overlay.css", css);

    return assets;
}

QWidget* ChannelPointPlugin::createWidget(QWidget* parent)
{
    if (!m_mainWidget) {
        m_mainWidget = new PluginMainWidget(m_context, m_effectMgr, m_queueMgr, m_analyticsMgr, parent);
        connect(m_mainWidget, &PluginMainWidget::testPlayRequested, this, &ChannelPointPlugin::onTestPlayRequested);
        connect(m_mainWidget, &PluginMainWidget::emergencyStopTriggered, this, &ChannelPointPlugin::onEmergencyStopTriggered);
    }
    return m_mainWidget;
}

void ChannelPointPlugin::onRewardRedeemed(const TwitchRewardRedemption& redemption)
{
    if (!m_effectMgr || !m_queueMgr || !m_analyticsMgr) return;

    RewardEffectSetting setting = m_effectMgr->getSetting(redemption.rewardId);
    if (!setting.enabled && !setting.rewardId.isEmpty()) {
        return; // 無効な報酬は無視
    }

    QueueItem qItem;
    qItem.redemptionId = redemption.id;
    qItem.rewardId = redemption.rewardId;
    qItem.rewardName = redemption.rewardName;
    qItem.userId = redemption.userId;
    qItem.username = redemption.username;
    qItem.displayName = redemption.displayName;
    qItem.userInput = redemption.userInput;
    qItem.timestamp = redemption.timestamp > 0 ? redemption.timestamp : QDateTime::currentSecsSinceEpoch();

    if (!setting.effects.isEmpty()) {
        qItem.targetEffectId = setting.effects.first().effectId;
    } else {
        qItem.targetEffectId = "effect_default";
    }

    // 1. アナリティクスへ記録
    m_analyticsMgr->recordRedemption(qItem, setting.points);

    // 2. Queue へ追加
    m_queueMgr->pushQueue(qItem);
}

void ChannelPointPlugin::onTestPlayRequested(const QString& rewardId)
{
    if (!m_queueMgr || !m_effectMgr) return;

    RewardEffectSetting setting = m_effectMgr->getSetting(rewardId);

    QueueItem qItem;
    qItem.redemptionId = QString("test_%1").arg(QDateTime::currentMSecsSinceEpoch());
    qItem.rewardId = rewardId;
    qItem.rewardName = setting.rewardName.isEmpty() ? "テスト報酬" : setting.rewardName;
    qItem.username = "Tester";
    qItem.displayName = "テスト配信者";
    qItem.userInput = "テスト再生メッセージ";
    qItem.timestamp = QDateTime::currentSecsSinceEpoch();
    qItem.targetEffectId = setting.effects.isEmpty() ? "effect_default" : setting.effects.first().effectId;

    m_queueMgr->pushQueue(qItem);
}

void ChannelPointPlugin::onEmergencyStopTriggered()
{
    if (m_queueMgr) {
        m_queueMgr->emergencyStop();
    }
}
