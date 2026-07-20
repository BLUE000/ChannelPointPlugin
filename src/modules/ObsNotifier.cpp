#include "ObsNotifier.h"
#include "../../tests/src/shared/plugin_interface.h"
#include <QDebug>

ObsNotifier::ObsNotifier(ICoreContext* context, QObject* parent)
    : QObject(parent)
    , m_context(context)
{
}

void ObsNotifier::sendRenderEffect(const QueueItem& queueItem, const EffectItem& effectItem, const QSize& canvasSize)
{
    if (!m_context) return;

    QJsonObject payload;
    payload["action"] = "render_effect";
    payload["redemption_id"] = queueItem.redemptionId;
    payload["reward_id"] = queueItem.rewardId;
    payload["reward_name"] = queueItem.rewardName;
    payload["user_name"] = queueItem.username;
    payload["display_name"] = queueItem.displayName;
    payload["user_input"] = queueItem.userInput;

    // 演出パラメータ
    QJsonObject effectObj;
    effectObj["effect_id"] = effectItem.effectId;
    effectObj["media_type"] = static_cast<int>(effectItem.mediaType);
    effectObj["image_path"] = effectItem.imagePath;
    effectObj["video_path"] = effectItem.videoPath;
    effectObj["audio_path"] = effectItem.audioPath;
    effectObj["duration_sec"] = effectItem.durationSec;
    effectObj["size_percent"] = effectItem.sizePercent;
    
    // 表示テキスト構築（変数の置き換え）
    QString text = effectItem.textTemplate;
    text.replace("{user}", queueItem.displayName.isEmpty() ? queueItem.username : queueItem.displayName);
    text.replace("{reward}", queueItem.rewardName);
    text.replace("{input}", queueItem.userInput);
    effectObj["display_text"] = text;

    effectObj["position_preset"] = effectItem.positionPreset;
    effectObj["center_x"] = effectItem.centerX;
    effectObj["center_y"] = effectItem.centerY;
    effectObj["is_custom_html"] = effectItem.isCustomHtml;

    payload["effect"] = effectObj;

    QJsonObject canvasObj;
    canvasObj["width"] = canvasSize.width();
    canvasObj["height"] = canvasSize.height();
    payload["canvas"] = canvasObj;

    // ICoreContext 経由で OBS 向け WebSocket / HTTP 送出
    m_context->sendToObs("channel_point_effect", payload);
}

void ObsNotifier::sendClearEffect(bool forceClear)
{
    if (!m_context) return;

    QJsonObject payload;
    payload["action"] = "clear_effect";
    payload["force"] = forceClear;

    m_context->sendToObs("channel_point_effect", payload);
}
