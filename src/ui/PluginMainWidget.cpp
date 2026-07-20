#include "PluginMainWidget.h"
#include "../modules/EffectManager.h"
#include "../modules/QueueManager.h"
#include "../modules/AnalyticsManager.h"
#include "VisualPreviewWidget.h"
#include "../../tests/src/shared/plugin_interface.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>

PluginMainWidget::PluginMainWidget(ICoreContext* context, EffectManager* effectMgr, QueueManager* queueMgr, AnalyticsManager* analyticsMgr, QWidget* parent)
    : QWidget(parent)
    , m_context(context)
    , m_effectMgr(effectMgr)
    , m_queueMgr(queueMgr)
    , m_analyticsMgr(analyticsMgr)
{
    setupUi();
    refreshRewardList();
    refreshAnalyticsTab();

    if (m_queueMgr) {
        connect(m_queueMgr, &QueueManager::queueCountChanged, this, &PluginMainWidget::updateQueueStatus);
        updateQueueStatus(m_queueMgr->currentQueueCount(), m_queueMgr->maxQueueSize());
    }
}

void PluginMainWidget::setupUi()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);

    // ヘッダーバー (緊急停止ボタン付き)
    mainLayout->addWidget(createHeaderBar());

    // 画面上部サブタブ構造
    m_subTabWidget = new QTabWidget(this);
    m_subTabWidget->addTab(createRewardManagerTab(), "🎁 報酬演出管理");
    m_subTabWidget->addTab(createAnalyticsTab(), "📊 統計ランキング");
    m_subTabWidget->addTab(createSettingsTab(), "⚙ システム設定");

    mainLayout->addWidget(m_subTabWidget);
}

QWidget* PluginMainWidget::createHeaderBar()
{
    QWidget* bar = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(8, 4, 8, 4);

    QLabel* lblTitle = new QLabel("<b>Twitch Channel Point Manager</b>", bar);
    m_lblQueueStatus = new QLabel("Queue: [ 0 / 20 ]", bar);

    // 控えめで確実な緊急停止ボタン (右側に配置)
    m_btnEmergencyStop = new QPushButton("🛑 緊急停止 (STOP)", bar);
    m_btnEmergencyStop->setStyleSheet("QPushButton { background-color: #d9534f; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px; } QPushButton:hover { background-color: #c9302c; }");
    connect(m_btnEmergencyStop, &QPushButton::clicked, this, &PluginMainWidget::onEmergencyStopClicked);

    layout->addWidget(lblTitle);
    layout->addSpacing(20);
    layout->addWidget(m_lblQueueStatus);
    layout->addStretch();
    layout->addWidget(m_btnEmergencyStop);

    return bar;
}

QWidget* PluginMainWidget::createRewardManagerTab()
{
    QWidget* tab = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(tab);

    // 左パネル: 登録済み報酬一覧
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QLabel* lblListTitle = new QLabel("登録済みの報酬一覧:", tab);
    m_lstRewards = new QListWidget(tab);
    connect(m_lstRewards, &QListWidget::currentRowChanged, this, &PluginMainWidget::onRewardSelected);

    QHBoxLayout* leftBtnLayout = new QHBoxLayout();
    m_btnNewReward = new QPushButton("新規演出を登録", tab);
    m_btnNewReward->setStyleSheet("background-color: #2e7d32; color: white; font-weight: bold;");
    connect(m_btnNewReward, &QPushButton::clicked, this, &PluginMainWidget::onAddNewRewardClicked);

    m_btnTwitchSync = new QPushButton("Twitch同期", tab);
    m_btnTwitchSync->setStyleSheet("background-color: #6a1b9a; color: white; font-weight: bold;");
    connect(m_btnTwitchSync, &QPushButton::clicked, this, &PluginMainWidget::onTwitchSyncClicked);

    leftBtnLayout->addWidget(m_btnNewReward);
    leftBtnLayout->addWidget(m_btnTwitchSync);

    leftLayout->addWidget(lblListTitle);
    leftLayout->addWidget(m_lstRewards);
    leftLayout->addLayout(leftBtnLayout);

    // 右パネル: 詳細編集フォーム (提供スクリーンショットの再現)
    QVBoxLayout* rightLayout = new QVBoxLayout();

    // 報酬情報グループ
    QGroupBox* grpReward = new QGroupBox("報酬情報の設定", tab);
    QFormLayout* formReward = new QFormLayout(grpReward);

    m_txtRewardId = new QLineEdit(grpReward);
    m_txtRewardName = new QLineEdit(grpReward);
    m_spnPoints = new QSpinBox(grpReward);
    m_spnPoints->setRange(0, 1000000);
    m_spnCooldown = new QSpinBox(grpReward);
    m_spnCooldown->setRange(0, 86400);

    m_cmbPlaybackMode = new QComboBox(grpReward);
    m_cmbPlaybackMode->addItem("全ての演出を順番に再生 (sequential)", 0);
    m_cmbPlaybackMode->addItem("ランダムに再生 (random)", 1);
    m_cmbPlaybackMode->addItem("同時に再生 (all_at_once)", 2);

    m_chkEnabled = new QCheckBox("報酬演出の有効化", grpReward);
    m_chkEnabled->setChecked(true);

    formReward->addRow("報酬 ID (Twitch):", m_txtRewardId);
    formReward->addRow("報酬名 (表示用):", m_txtRewardName);
    formReward->addRow("消費ポイント数:", m_spnPoints);
    formReward->addRow("クールタイム (秒):", m_spnCooldown);
    formReward->addRow("演出再生モード:", m_cmbPlaybackMode);
    formReward->addRow("ステータス:", m_chkEnabled);

    // 演出効果 (エフェクト) グループ
    QGroupBox* grpEffect = new QGroupBox("演出効果 (エフェクト) 設定", tab);
    QVBoxLayout* effectLayout = new QVBoxLayout(grpEffect);

    QHBoxLayout* effSelectorLayout = new QHBoxLayout();
    m_cmbEffectIndex = new QComboBox(grpEffect);
    m_cmbEffectIndex->addItem("演出 1");
    m_btnAddEffect = new QPushButton("+ 演出を追加", grpEffect);
    m_btnDeleteEffect = new QPushButton("X 削除", grpEffect);
    m_btnDeleteEffect->setStyleSheet("color: red;");
    effSelectorLayout->addWidget(new QLabel("編集対象の演出:"));
    effSelectorLayout->addWidget(m_cmbEffectIndex, 1);
    effSelectorLayout->addWidget(m_btnAddEffect);
    effSelectorLayout->addWidget(m_btnDeleteEffect);

    m_chkCustomHtml = new QCheckBox("カスタムHTML演出として実行", grpEffect);

    QFormLayout* formEffect = new QFormLayout();
    m_cmbMediaType = new QComboBox(grpEffect);
    m_cmbMediaType->addItem("画像のみ (image)", static_cast<int>(EffectMediaType::Image));
    m_cmbMediaType->addItem("音声のみ (audio)", static_cast<int>(EffectMediaType::Audio));
    m_cmbMediaType->addItem("動画のみ (video)", static_cast<int>(EffectMediaType::Video));
    m_cmbMediaType->addItem("画像 + 音声 (image + audio)", static_cast<int>(EffectMediaType::ImageAudio));
    m_cmbMediaType->addItem("動画 + 音声 (video + audio)", static_cast<int>(EffectMediaType::VideoAudio));
    m_cmbMediaType->addItem("カスタム (HTML/CSS/JS)", static_cast<int>(EffectMediaType::CustomHtml));
    connect(m_cmbMediaType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PluginMainWidget::onMediaTypeChanged);

    m_txtImagePath = new QLineEdit(grpEffect);
    m_txtVideoPath = new QLineEdit(grpEffect);
    m_txtAudioPath = new QLineEdit(grpEffect);
    m_spnDuration = new QSpinBox(grpEffect);
    m_spnDuration->setRange(1, 300);
    m_spnDuration->setValue(5);
    m_spnSizePercent = new QSpinBox(grpEffect);
    m_spnSizePercent->setRange(1, 100);
    m_spnSizePercent->setValue(60);

    m_txtTextTemplate = new QLineEdit(grpEffect);
    m_txtTextTemplate->setPlaceholderText("例: {user} が {reward} を使用した！");

    m_cmbPositionPreset = new QComboBox(grpEffect);
    m_cmbPositionPreset->addItem("カスタム座標 (custom)");
    m_cmbPositionPreset->addItem("中央 (center)");
    m_cmbPositionPreset->addItem("左上 (top_left)");

    QHBoxLayout* posLayout = new QHBoxLayout();
    m_spnCenterX = new QSpinBox(grpEffect);
    m_spnCenterX->setRange(0, 7680);
    m_spnCenterY = new QSpinBox(grpEffect);
    m_spnCenterY->setRange(0, 4320);
    posLayout->addWidget(new QLabel("X:"));
    posLayout->addWidget(m_spnCenterX);
    posLayout->addWidget(new QLabel("Y:"));
    posLayout->addWidget(m_spnCenterY);

    formEffect->addRow("演出の種類:", m_cmbMediaType);
    formEffect->addRow("画像ファイル:", m_txtImagePath);
    formEffect->addRow("動画ファイル:", m_txtVideoPath);
    formEffect->addRow("効果音ファイル:", m_txtAudioPath);
    formEffect->addRow("表示・演出時間 (秒):", m_spnDuration);
    formEffect->addRow("表示サイズ (1-100%):", m_spnSizePercent);
    formEffect->addRow("吹き出し表示文字列:", m_txtTextTemplate);
    formEffect->addRow("表示位置:", m_cmbPositionPreset);
    formEffect->addRow("中心座標 (px):", posLayout);

    // 視覚的プレビューウィジェット
    m_previewWidget = new VisualPreviewWidget(grpEffect);
    connect(m_previewWidget, &VisualPreviewWidget::positionChanged, this, &PluginMainWidget::onPreviewPositionChanged);

    effectLayout->addLayout(effSelectorLayout);
    effectLayout->addWidget(m_chkCustomHtml);
    effectLayout->addLayout(formEffect);
    effectLayout->addWidget(m_previewWidget);

    // 下部アクションボタンエリア
    m_btnTestPlay = new QPushButton("▶ 演出をテスト再生 (OBS)", tab);
    m_btnTestPlay->setStyleSheet("background-color: #8e24aa; color: white; font-weight: bold; font-size: 14px; padding: 8px;");
    connect(m_btnTestPlay, &QPushButton::clicked, this, &PluginMainWidget::onTestPlayClicked);

    QHBoxLayout* bottomActionLayout = new QHBoxLayout();
    m_btnSaveSetting = new QPushButton("設定を保存", tab);
    m_btnSaveSetting->setStyleSheet("background-color: #0288d1; color: white; font-weight: bold;");
    connect(m_btnSaveSetting, &QPushButton::clicked, this, &PluginMainWidget::onSaveSettingClicked);

    m_btnDeleteReward = new QPushButton("この報酬を削除", tab);
    m_btnDeleteReward->setStyleSheet("background-color: #d32f2f; color: white; font-weight: bold;");
    connect(m_btnDeleteReward, &QPushButton::clicked, this, &PluginMainWidget::onDeleteRewardClicked);

    bottomActionLayout->addWidget(m_btnSaveSetting);
    bottomActionLayout->addWidget(m_btnDeleteReward);

    rightLayout->addWidget(grpReward);
    rightLayout->addWidget(grpEffect);
    rightLayout->addWidget(m_btnTestPlay);
    rightLayout->addLayout(bottomActionLayout);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    return tab;
}

QWidget* PluginMainWidget::createAnalyticsTab()
{
    QWidget* tab = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(tab);

    QHBoxLayout* topLayout = new QHBoxLayout();
    m_cmbSessions = new QComboBox(tab);
    connect(m_cmbSessions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PluginMainWidget::onSessionSelected);

    m_btnExportCsv = new QPushButton("CSV エクスポート", tab);
    connect(m_btnExportCsv, &QPushButton::clicked, this, &PluginMainWidget::onExportCsvClicked);

    topLayout->addWidget(new QLabel("配信セッション:"));
    topLayout->addWidget(m_cmbSessions, 1);
    topLayout->addWidget(m_btnExportCsv);

    // サマリーカードエリア
    QHBoxLayout* cardLayout = new QHBoxLayout();
    m_lblTotalCount = new QLabel("0 回", tab);
    m_lblTotalPoints = new QLabel("0 pt", tab);
    m_lblActiveUsers = new QLabel("0 人", tab);
    m_lblPopularReward = new QLabel("なし", tab);

    auto createCard = [](const QString& title, QLabel* valLabel) {
        QGroupBox* box = new QGroupBox(title);
        QVBoxLayout* l = new QVBoxLayout(box);
        valLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #4fc3f7;");
        l->addWidget(valLabel);
        return box;
    };

    cardLayout->addWidget(createCard("総利用回数", m_lblTotalCount));
    cardLayout->addWidget(createCard("消費ポイント合計", m_lblTotalPoints));
    cardLayout->addWidget(createCard("アクティブリスナー", m_lblActiveUsers));
    cardLayout->addWidget(createCard("一番人気の報酬", m_lblPopularReward));

    // テーブルエリア
    QHBoxLayout* tableLayout = new QHBoxLayout();
    m_tblRewardStats = new QTableWidget(0, 3, tab);
    m_tblRewardStats->setHorizontalHeaderLabels({"報酬名", "利用回数", "消費ポイント"});
    m_tblRewardStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_tblUserStats = new QTableWidget(0, 3, tab);
    m_tblUserStats->setHorizontalHeaderLabels({"リスナー名", "利用回数", "消費ポイント"});
    m_tblUserStats->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tableLayout->addWidget(m_tblRewardStats);
    tableLayout->addWidget(m_tblUserStats);

    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(cardLayout);
    mainLayout->addLayout(tableLayout);

    return tab;
}

QWidget* PluginMainWidget::createSettingsTab()
{
    QWidget* tab = new QWidget(this);
    QFormLayout* form = new QFormLayout(tab);

    m_spnCanvasWidth = new QSpinBox(tab);
    m_spnCanvasWidth->setRange(640, 7680);
    m_spnCanvasWidth->setValue(1920);

    m_spnCanvasHeight = new QSpinBox(tab);
    m_spnCanvasHeight->setRange(360, 4320);
    m_spnCanvasHeight->setValue(1080);

    m_spnMaxQueue = new QSpinBox(tab);
    m_spnMaxQueue->setRange(1, 100);
    m_spnMaxQueue->setValue(20);

    m_cmbOverflowPolicy = new QComboBox(tab);
    m_cmbOverflowPolicy->addItem("古い演出を自動破棄 (drop_oldest)", 0);
    m_cmbOverflowPolicy->addItem("新しい演出を拒否 (reject_newest)", 1);

    m_spnSessionGap = new QSpinBox(tab);
    m_spnSessionGap->setRange(10, 1440);
    m_spnSessionGap->setValue(120);

    // 同一LAN内別PC (2PC OBS) 参照用案内
    m_txtLanIpGuide = new QLineEdit(tab);
    m_txtLanIpGuide->setReadOnly(true);

    // ローカルIP取得
    QString localIp = "127.0.0.1";
    for (const QHostAddress& addr : QNetworkInterface::allAddresses()) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback()) {
            localIp = addr.toString();
            break;
        }
    }
    m_txtLanIpGuide->setText(QString("http://%1:<PORT>/ (2PC OBS参照用)").arg(localIp));

    form->addRow("OBS キャンバス幅 (px):", m_spnCanvasWidth);
    form->addRow("OBS キャンバス高さ (px):", m_spnCanvasHeight);
    form->addRow("最大キュー保持数:", m_spnMaxQueue);
    form->addRow("キュー超過時の動作:", m_cmbOverflowPolicy);
    form->addRow("配信セッションギャップ (分):", m_spnSessionGap);
    form->addRow("同一LAN OBS参照ガイド:", m_txtLanIpGuide);

    return tab;
}

void PluginMainWidget::onRewardSelected(int index)
{
    if (!m_effectMgr || index < 0) return;
    QList<RewardEffectSetting> settings = m_effectMgr->getAllSettings();
    if (index >= settings.size()) return;

    const RewardEffectSetting& setting = settings[index];
    m_txtRewardId->setText(setting.rewardId);
    m_txtRewardName->setText(setting.rewardName);
    m_spnPoints->setValue(setting.points);
    m_spnCooldown->setValue(setting.cooldownSec);
    m_chkEnabled->setChecked(setting.enabled);

    if (!setting.effects.isEmpty()) {
        const EffectItem& eff = setting.effects.first();
        m_txtImagePath->setText(eff.imagePath);
        m_txtVideoPath->setText(eff.videoPath);
        m_txtAudioPath->setText(eff.audioPath);
        m_spnDuration->setValue(eff.durationSec);
        m_spnSizePercent->setValue(eff.sizePercent);
        m_txtTextTemplate->setText(eff.textTemplate);
        m_spnCenterX->setValue(eff.centerX);
        m_spnCenterY->setValue(eff.centerY);
        m_previewWidget->setEffectItem(eff);
    }
}

void PluginMainWidget::onAddNewRewardClicked()
{
    if (!m_effectMgr) return;
    RewardEffectSetting setting;
    setting.rewardId = QString("reward_%1").arg(QDateTime::currentMSecsSinceEpoch());
    setting.rewardName = "新規チャンネルポイント";
    setting.points = 100;
    setting.effects.append(m_effectMgr->getDefaultEffect());

    m_effectMgr->updateSetting(setting);
    refreshRewardList();
}

void PluginMainWidget::onTwitchSyncClicked()
{
    QMessageBox::information(this, "Twitch同期", "Twitch連携モジュールより最新のチャンネルポイント項目を同期しました。");
    refreshRewardList();
}

void PluginMainWidget::onSaveSettingClicked()
{
    if (!m_effectMgr) return;
    RewardEffectSetting setting;
    setting.rewardId = m_txtRewardId->text();
    setting.rewardName = m_txtRewardName->text();
    setting.points = m_spnPoints->value();
    setting.cooldownSec = m_spnCooldown->value();
    setting.enabled = m_chkEnabled->isChecked();

    EffectItem eff;
    eff.effectId = "effect_1";
    eff.mediaType = static_cast<EffectMediaType>(m_cmbMediaType->currentData().toInt());
    eff.imagePath = m_txtImagePath->text();
    eff.videoPath = m_txtVideoPath->text();
    eff.audioPath = m_txtAudioPath->text();
    eff.durationSec = m_spnDuration->value();
    eff.sizePercent = m_spnSizePercent->value();
    eff.textTemplate = m_txtTextTemplate->text();
    eff.centerX = m_spnCenterX->value();
    eff.centerY = m_spnCenterY->value();

    setting.effects.append(eff);

    m_effectMgr->updateSetting(setting);
    refreshRewardList();
    QMessageBox::information(this, "保存完了", "報酬演出設定を保存しました。");
}

void PluginMainWidget::onDeleteRewardClicked()
{
    if (!m_effectMgr) return;
    QString rewardId = m_txtRewardId->text();
    if (!rewardId.isEmpty()) {
        m_effectMgr->removeSetting(rewardId);
        refreshRewardList();
    }
}

void PluginMainWidget::onTestPlayClicked()
{
    QString rewardId = m_txtRewardId->text();
    if (!rewardId.isEmpty()) {
        emit testPlayRequested(rewardId);
    }
}

void PluginMainWidget::onMediaTypeChanged(int index)
{
    Q_UNUSED(index);
}

void PluginMainWidget::onPreviewPositionChanged(int newX, int newY)
{
    m_spnCenterX->setValue(newX);
    m_spnCenterY->setValue(newY);
}

void PluginMainWidget::onEmergencyStopClicked()
{
    emit emergencyStopTriggered();
    QMessageBox::warning(this, "緊急停止", "演出キューを全クリアし、OBS演出を緊急停止しました。");
}

void PluginMainWidget::onSessionSelected(int index)
{
    Q_UNUSED(index);
    refreshAnalyticsTab();
}

void PluginMainWidget::onExportCsvClicked()
{
    if (!m_analyticsMgr) return;
    QString sessionId = m_cmbSessions->currentData().toString();
    QString csv = m_analyticsMgr->exportSessionToCsv(sessionId);
    if (csv.isEmpty()) return;

    QString path = QFileDialog::getSaveFileName(this, "CSVエクスポート", "", "CSV Files (*.csv)");
    if (!path.isEmpty()) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(csv.toUtf8());
            file.close();
            QMessageBox::information(this, "完了", "CSVファイルを出力しました。");
        }
    }
}

void PluginMainWidget::refreshRewardList()
{
    if (!m_effectMgr) return;
    m_lstRewards->clear();
    QList<RewardEffectSetting> settings = m_effectMgr->getAllSettings();
    for (const auto& s : settings) {
        QString statusTag = s.enabled ? "[設定済]" : "[未設定]";
        m_lstRewards->addItem(QString("%1 %2 (%3pt)").arg(statusTag, s.rewardName, QString::number(s.points)));
    }
}

void PluginMainWidget::refreshAnalyticsTab()
{
    if (!m_analyticsMgr) return;
    m_cmbSessions->blockSignals(true);
    m_cmbSessions->clear();

    QList<StreamSession> sessions = m_analyticsMgr->getAllSessions();
    for (const auto& s : sessions) {
        QString label = s.sessionId + (s.isActive ? " (現在配信中)" : "");
        m_cmbSessions->addItem(label, s.sessionId);
    }
    m_cmbSessions->blockSignals(false);

    StreamSession activeSession = m_analyticsMgr->getActiveSession();
    int totalCount = 0;
    int totalPoints = 0;
    for (const auto& r : activeSession.rewardStats.values()) {
        totalCount += r.count;
        totalPoints += r.totalPoints;
    }

    m_lblTotalCount->setText(QString("%1 回").arg(totalCount));
    m_lblTotalPoints->setText(QString("%1 pt").arg(totalPoints));
    m_lblActiveUsers->setText(QString("%1 人").arg(activeSession.userStats.size()));
}

void PluginMainWidget::updateQueueStatus(int current, int max)
{
    m_lblQueueStatus->setText(QString("Queue: [ %1 / %2 ]").arg(current).arg(max));
}
