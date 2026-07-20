# 詳細設計書 (内部設計・C++クラス構造設計) - ChannelPointPlugin

## 1. 概要
本ドキュメントは、基本設計書 (`doc/02_BasicDesignSpecification.md`) に基づき、プラグイン内部のデータ型、C++クラス定義、メソッドインターフェース、アルゴリズム、IP/ポート非ハードコーディング規定、2PC配信・同一LAN内接続対応規定、およびOBS配信用アセットの具体的な内部仕様を定義する詳細設計書です。

---

## 2. ネットワーク通信・IP/ポート非ハードコーディング & LAN内接続規定

### 2.1 ハードコーディング禁止方針
- **IPアドレスおよびポート番号のコード固定禁止**: C++ソースコードおよびJavaScriptアセット内に、IPアドレス (`127.0.0.1`, `localhost` 等) やポート番号をハードコーディングしてはならない。
- **ホストアプリAPI委譲**: OBS（ブラウザソース）へのメッセージ送出は `ICoreContext::sendToObs` を介して行い、ソケットバインドやネットワークアドレスの解決はすべてホストアプリコア側に一任する。
- **動的設定取得**: 万一プラグイン側で個別ソケット通信や特定ポート参照が必要となる場合は、すべて設定ファイル (`effect_settings.json`) またはホストアプリの `ICoreContext` から動的に取得・変更できる構成とする。

### 2.2 2PC配信・同一LAN内別PC（外部OBS）接続対応
- **ローカルループバック固定の排除**: ループバック (`127.0.0.1`) 固定を排除し、ネットワークポートが開口している環境であれば同一LAN内の別PC（例: 配信専用PCで稼働するOBS）からの接続・演出受信を完全にサポートする。
- **自IP（LAN IP）表示・案内**: プラグインUI（システム設定/ヘルプタブ）にて、`QNetworkInterface` により検出したホストPCのローカルIPアドレス（例: `http://192.168.x.x:port/`）を表示し、別PCのOBSから簡単に接続URLを設定・コピーできるガイド機能を提供する。

---

## 3. データ型・構造体詳細定義 (Data Structures)

### 3.1 列挙型 (Enums)

```cpp
enum class EffectMediaType {
    Image,          // 画像のみ
    Audio,          // 音声のみ
    Video,          // 動画のみ
    ImageAudio,     // 画像 + 音声
    VideoAudio,     // 動画 + 音声
    CustomHtml      // カスタム HTML / CSS / JS
};

enum class PlaybackMode {
    Sequential,     // 順番に再生
    Random,         // ランダム再生
    AllAtOnce       // 同時再生
};

enum class QueueOverflowPolicy {
    DropOldest,     // 古い演出を破棄
    RejectNewest    // 新しい演出を破棄
};
```

### 3.2 演出アイテムおよび報酬設定構造体

```cpp
// 演出単体の詳細設定
struct EffectItem {
    QString effectId;               // 演出ID (例: "effect_1")
    EffectMediaType mediaType;      // 演出の媒体種別
    QString imagePath;              // 画像ファイルパス
    QString videoPath;              // 動画ファイルパス
    QString audioPath;              // 効果音ファイルパス
    int durationSec = 5;            // 表示・演出時間 (秒)
    int sizePercent = 60;           // 表示サイズ (1 - 100 %)
    QString textTemplate;           // 表示テキスト (例: "{user} が {reward} を使用！")
    QString positionPreset;         // 表示位置プリセット ("top_left", "center", "custom" 等)
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
    qint64 timestamp;               // 受信タイムスタンプ
    QString targetEffectId;         // 対象の演出ID (参照用)
};
```

---

## 4. C++ クラス定義詳細 (Class Specifications)

### 4.1 `ChannelPointPlugin` (メインエントリーポイント)
`plugin_interface.h` の `IChannelPlugin` を継承・実装。

```cpp
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
    virtual QByteArray iconPngData() const override; // pic/ChannelPoint.png を返却
    virtual QMap<QString, QByteArray> defaultAssets() const override;
    virtual QWidget* createWidget(QWidget* parent = nullptr) override;

    // イベント通知受信関数
    virtual void onRewardRedemptionReceived(const TwitchRewardRedemption& redemption) override;

private:
    ICoreContext* m_context = nullptr;
    EffectManager* m_effectMgr = nullptr;
    QueueManager* m_queueMgr = nullptr;
    AnalyticsManager* m_analyticsMgr = nullptr;
    ObsNotifier* m_obsNotifier = nullptr;
    PluginMainWidget* m_mainWidget = nullptr;
};
```

### 4.2 `EffectManager`
演出マスター設定のロード/セーブ、報酬マッピングの検索。

```cpp
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

    // 規定OBS解像度
    QSize getObsCanvasSize() const { return m_obsCanvasSize; }
    void setObsCanvasSize(const QSize& size);

private:
    ICoreContext* m_context;
    QSize m_obsCanvasSize{1920, 1080};
    QMap<QString, RewardEffectSetting> m_settingsMap;
};
```

### 4.3 `QueueManager` (キュー制御 & 緊急停止)

```cpp
class QueueManager : public QObject {
    Q_OBJECT
public:
    explicit QueueManager(EffectManager* effectMgr, ObsNotifier* obsNotifier, QObject* parent = nullptr);

    void pushQueue(const QueueItem& item);
    void emergencyStop(); // 緊急停止 (クリア + OBS即時消去)
    int currentQueueCount() const { return m_queue.size(); }
    int maxQueueSize() const { return m_maxQueueSize; }

signals:
    void queueCountChanged(int count, int maxCount);

private slots:
    void processNext();

private:
    EffectManager* m_effectMgr;
    ObsNotifier* m_obsNotifier;
    QQueue<QueueItem> m_queue;
    QTimer* m_playbackTimer;
    bool m_isPlaying = false;
    int m_maxQueueSize = 20;
};
```

### 4.4 `VisualPreviewWidget` (はみ出し境界チェック & ミニプレビュー)

```cpp
class VisualPreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit VisualPreviewWidget(QWidget* parent = nullptr);
    void setObsCanvasSize(const QSize& canvasSize);
    void setEffectItem(const EffectItem& item);

    // はみ出しチェック & 修正アルゴリズム
    struct BoundaryResult {
        bool isOutOfBounds;
        int clampedX;
        int clampedY;
        QString warningMessage;
    };
    BoundaryResult checkBoundary(int x, int y, int sizePercent, const QSize& canvasSize) const;

signals:
    void positionChanged(int newX, int newY);

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    QSize m_canvasSize{1920, 1080};
    EffectItem m_currentEffect;
    bool m_isDragging = false;
};
```

---

## 5. はみ出しチェック (Boundary Check) アルゴリズム詳細

基準画面幅 $W$, 高さ $H$（例: $1920 \times 1080$）に対し、演出アイテムの標準矩形サイズを $W_{elem} = W \times \frac{\text{sizePercent}}{100}$, $H_{elem} = H \times \frac{\text{sizePercent}}{100}$ とする。

中心座標 $(X_{center}, Y_{center})$ におけるバウンディングボックスの四隅座標：
$$X_{min} = X_{center} - \frac{W_{elem}}{2}, \quad X_{max} = X_{center} + \frac{W_{elem}}{2}$$
$$Y_{min} = Y_{center} - \frac{H_{elem}}{2}, \quad Y_{max} = Y_{center} + \frac{H_{elem}}{2}$$

**判定条件**:
1. **完全画面外**: $X_{max} < 0 \lor X_{min} > W \lor Y_{max} < 0 \lor Y_{min} > H$ の場合、はみ出し（OutOfBounds = true）。
2. **自動クランピング補正**:
   $$X_{clamped} = \text{clamp}\left(X_{center}, \frac{W_{elem}}{2}, W - \frac{W_{elem}}{2}\right)$$
   $$Y_{clamped} = \text{clamp}\left(Y_{center}, \frac{H_{elem}}{2}, H - \frac{H_{elem}}{2}\right)$$

---

## 6. OBSブラウザソース埋め込みアセット構造 (`defaultAssets()`)

プラグイン組み込みの静的Webアセット：

1. **`assets/overlay.html`**: OBSにブラウザソースとして読み込ませるメインページ。
2. **`assets/overlay.js`**: WebSocketイベント (`render_effect`, `clear_effect`) を受信し、画像/動画/音声アニメーションをスムーズに画面表示するスクリプト（接続先IP/ポートは動的に自動判定、別PCからの接続時も相対パスまたは動的アドレス解決で通信動作）。
3. **`assets/overlay.css`**: ポップイン・フェードイン等のCSS3アニメーションスタイルシート。

---

## 7. 変更履歴
- **2026-07-20 (v1.2)**: 2PC配信構成・同一LAN内別PCからの外部OBS接続対応規定を明記。
- **2026-07-20 (v1.1)**: IPアドレスおよびポート番号のハードコーディング禁止規定（`ICoreContext::sendToObs` 委譲・設定ファイル動的取得）を追加。
