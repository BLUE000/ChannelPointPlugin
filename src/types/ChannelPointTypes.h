#ifndef CHANNEL_POINT_TYPES_H
#define CHANNEL_POINT_TYPES_H

#include <QString>
#include <QList>
#include <QMap>
#include <QSize>
#include <QJsonObject>
#include <QJsonArray>
#include <QtGlobal>

// 演出の媒体種別
enum class EffectMediaType {
    Image,          // 画像のみ
    Audio,          // 音声のみ
    Video,          // 動画のみ
    ImageAudio,     // 画像 + 音声
    VideoAudio,     // 動画 + 音声
    CustomHtml      // カスタム HTML / CSS / JS
};

// 演出再生モード
enum class PlaybackMode {
    Sequential,     // 順番に再生
    Random,         // ランダム再生
    AllAtOnce       // 同時再生
};

// キュー超過時の動作ポリシー
enum class QueueOverflowPolicy {
    DropOldest,     // 古い演出を破棄
    RejectNewest    // 新しい演出を破棄
};

// 演出単体の詳細設定
struct EffectItem {
    QString effectId;               // 演出ID (例: "effect_1")
    EffectMediaType mediaType = EffectMediaType::ImageAudio; // 媒体種別
    QString imagePath;              // 画像ファイルパス
    QString videoPath;              // 動画ファイルパス
    QString audioPath;              // 効果音ファイルパス
    int durationSec = 5;            // 表示・演出時間 (秒)
    int sizePercent = 60;           // 表示サイズ (1 - 100 %)
    QString textTemplate;           // 表示テキスト (例: "{user} が {reward} を使用！")
    QString positionPreset = "custom"; // 表示位置プリセット ("top_left", "center", "custom" 等)
    int centerX = 200;              // X中心座標 (px)
    int centerY = 150;              // Y中心座標 (px)
    bool isCustomHtml = false;      // カスタムHTML実行フラグ
};

// Twitch報酬項目とのマッピング設定
struct RewardEffectSetting {
    QString rewardId;               // Twitch 報酬 ID
    QString rewardName;             // 報酬名
    int points = 0;                 // 消費ポイント数
    int cooldownSec = 0;            // クールタイム (秒)
    bool enabled = true;            // 有効 / 無効フラグ
    PlaybackMode playbackMode = PlaybackMode::Sequential;
    QList<EffectItem> effects;      // 登録されている演出リスト
};

// ID参照型軽量キューアイテム
struct QueueItem {
    QString redemptionId;           // 引き換えイベントID
    QString rewardId;               // 報酬ID
    QString rewardName;             // 報酬名
    QString userId;                 // ユーザーID
    QString username;               // アカウント名
    QString displayName;            // 表示名
    QString userInput;              // 入力メッセージ
    qint64 timestamp = 0;           // 受信タイムスタンプ
    QString targetEffectId;         // 対象の演出ID (参照用)
};

// 報酬別統計データ
struct RewardStat {
    QString rewardId;
    QString rewardName;
    int count = 0;
    int totalPoints = 0;
};

// ユーザー別統計データ
struct UserStat {
    QString userId;
    QString username;
    QString displayName;
    int count = 0;
    int totalPoints = 0;
};

// 配信セッションデータ
struct StreamSession {
    QString sessionId;
    qint64 startTime = 0;
    qint64 endTime = 0;
    bool isActive = true;
    QMap<QString, RewardStat> rewardStats;
    QMap<QString, UserStat> userStats;
};

#endif // CHANNEL_POINT_TYPES_H
