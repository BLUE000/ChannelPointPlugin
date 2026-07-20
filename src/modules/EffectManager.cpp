#include "EffectManager.h"
#include "../../../tests/src/shared/plugin_interface.h"
#include <QFile>
#include <QDir>
#include <QDebug>

EffectManager::EffectManager(ICoreContext* context, QObject* parent)
    : QObject(parent)
    , m_context(context)
{
    loadSettings();
}

bool EffectManager::loadSettings()
{
    if (!m_context) return false;

    QByteArray data = m_context->readEncryptedFile("effect_settings.json");
    if (data.isEmpty()) {
        // 設定データがない場合は初期状態を保持
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Failed to parse effect_settings.json:" << parseError.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    
    // OBS キャンバス解像度
    if (root.contains("obs_canvas") && root["obs_canvas"].isObject()) {
        QJsonObject canvas = root["obs_canvas"].toObject();
        int w = canvas.value("width").toInt(1920);
        int h = canvas.value("height").toInt(1080);
        m_obsCanvasSize = QSize(w, h);
    }

    m_maxQueueSize = root.value("max_queue_size").toInt(20);
    m_sessionGapMinutes = root.value("session_gap_minutes").toInt(120);

    QString policyStr = root.value("overflow_policy").toString("drop_oldest");
    m_overflowPolicy = (policyStr == "reject_newest") ? QueueOverflowPolicy::RejectNewest : QueueOverflowPolicy::DropOldest;

    m_settingsMap.clear();
    QJsonArray settingsArr = root.value("reward_settings").toArray();
    for (const QJsonValue& val : settingsArr) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();
        RewardEffectSetting setting;
        setting.rewardId = obj.value("reward_id").toString();
        setting.rewardName = obj.value("reward_name").toString();
        setting.points = obj.value("points").toInt();
        setting.cooldownSec = obj.value("cooldown_sec").toInt();
        setting.enabled = obj.value("enabled").toBool(true);

        QString modeStr = obj.value("playback_mode").toString("sequential");
        if (modeStr == "random") setting.playbackMode = PlaybackMode::Random;
        else if (modeStr == "all_at_once") setting.playbackMode = PlaybackMode::AllAtOnce;
        else setting.playbackMode = PlaybackMode::Sequential;

        QJsonArray effectsArr = obj.value("effects").toArray();
        for (const QJsonValue& effVal : effectsArr) {
            if (!effVal.isObject()) continue;
            QJsonObject effObj = effVal.toObject();
            EffectItem item;
            item.effectId = effObj.value("effect_id").toString();
            item.imagePath = effObj.value("image_path").toString();
            item.videoPath = effObj.value("video_path").toString();
            item.audioPath = effObj.value("audio_path").toString();
            item.durationSec = effObj.value("duration_sec").toInt(5);
            item.sizePercent = effObj.value("size_percent").toInt(60);
            item.textTemplate = effObj.value("text_template").toString();
            item.positionPreset = effObj.value("position_preset").toString("custom");
            item.centerX = effObj.value("center_x").toInt(200);
            item.centerY = effObj.value("center_y").toInt(150);
            item.isCustomHtml = effObj.value("is_custom_html").toBool(false);

            int mediaTypeInt = effObj.value("media_type").toInt(static_cast<int>(EffectMediaType::ImageAudio));
            item.mediaType = static_cast<EffectMediaType>(mediaTypeInt);

            setting.effects.append(item);
        }

        if (!setting.rewardId.isEmpty()) {
            m_settingsMap.insert(setting.rewardId, setting);
        }
    }

    emit settingsUpdated();
    return true;
}

bool EffectManager::saveSettings()
{
    if (!m_context) return false;

    QJsonObject root;
    QJsonObject canvasObj;
    canvasObj["width"] = m_obsCanvasSize.width();
    canvasObj["height"] = m_obsCanvasSize.height();
    root["obs_canvas"] = canvasObj;

    root["max_queue_size"] = m_maxQueueSize;
    root["session_gap_minutes"] = m_sessionGapMinutes;
    root["overflow_policy"] = (m_overflowPolicy == QueueOverflowPolicy::RejectNewest) ? "reject_newest" : "drop_oldest";

    QJsonArray settingsArr;
    for (const RewardEffectSetting& setting : m_settingsMap.values()) {
        QJsonObject obj;
        obj["reward_id"] = setting.rewardId;
        obj["reward_name"] = setting.rewardName;
        obj["points"] = setting.points;
        obj["cooldown_sec"] = setting.cooldownSec;
        obj["enabled"] = setting.enabled;

        QString modeStr = "sequential";
        if (setting.playbackMode == PlaybackMode::Random) modeStr = "random";
        else if (setting.playbackMode == PlaybackMode::AllAtOnce) modeStr = "all_at_once";
        obj["playback_mode"] = modeStr;

        QJsonArray effectsArr;
        for (const EffectItem& item : setting.effects) {
            QJsonObject effObj;
            effObj["effect_id"] = item.effectId;
            effObj["media_type"] = static_cast<int>(item.mediaType);
            effObj["image_path"] = item.imagePath;
            effObj["video_path"] = item.videoPath;
            effObj["audio_path"] = item.audioPath;
            effObj["duration_sec"] = item.durationSec;
            effObj["size_percent"] = item.sizePercent;
            effObj["text_template"] = item.textTemplate;
            effObj["position_preset"] = item.positionPreset;
            effObj["center_x"] = item.centerX;
            effObj["center_y"] = item.centerY;
            effObj["is_custom_html"] = item.isCustomHtml;
            effectsArr.append(effObj);
        }
        obj["effects"] = effectsArr;
        settingsArr.append(obj);
    }
    root["reward_settings"] = settingsArr;

    QJsonDocument doc(root);
    QByteArray jsonData = doc.toJson(QJsonDocument::Indented);

    bool ok = m_context->writeEncryptedFile("effect_settings.json", jsonData);
    if (ok) {
        emit settingsUpdated();
    }
    return ok;
}

QList<RewardEffectSetting> EffectManager::getAllSettings() const
{
    return m_settingsMap.values();
}

RewardEffectSetting EffectManager::getSetting(const QString& rewardId) const
{
    return m_settingsMap.value(rewardId, RewardEffectSetting{});
}

void EffectManager::updateSetting(const RewardEffectSetting& setting)
{
    if (setting.rewardId.isEmpty()) return;
    m_settingsMap.insert(setting.rewardId, setting);
    saveSettings();
}

void EffectManager::removeSetting(const QString& rewardId)
{
    if (m_settingsMap.remove(rewardId) > 0) {
        saveSettings();
    }
}

void EffectManager::setObsCanvasSize(const QSize& size)
{
    if (size.width() > 0 && size.height() > 0) {
        m_obsCanvasSize = size;
        saveSettings();
    }
}

void EffectManager::setMaxQueueSize(int size)
{
    if (size > 0) {
        m_maxQueueSize = size;
        saveSettings();
    }
}

void EffectManager::setOverflowPolicy(QueueOverflowPolicy policy)
{
    m_overflowPolicy = policy;
    saveSettings();
}

void EffectManager::setSessionGapMinutes(int minutes)
{
    if (minutes > 0) {
        m_sessionGapMinutes = minutes;
        saveSettings();
    }
}

EffectItem EffectManager::getDefaultEffect() const
{
    EffectItem item;
    item.effectId = "effect_default";
    item.mediaType = EffectMediaType::ImageAudio;
    item.durationSec = 5;
    item.sizePercent = 60;
    item.textTemplate = "{user} が {reward} を使用しました！";
    item.positionPreset = "center";
    item.centerX = m_obsCanvasSize.width() / 2;
    item.centerY = m_obsCanvasSize.height() / 2;
    return item;
}
