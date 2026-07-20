#ifndef PLUGIN_MAIN_WIDGET_H
#define PLUGIN_MAIN_WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QProgressBar>
#include "../types/ChannelPointTypes.h"

class EffectManager;
class QueueManager;
class AnalyticsManager;
class VisualPreviewWidget;
class ICoreContext;

class PluginMainWidget : public QWidget {
    Q_OBJECT
public:
    explicit PluginMainWidget(ICoreContext* context, EffectManager* effectMgr, QueueManager* queueMgr, AnalyticsManager* analyticsMgr, QWidget* parent = nullptr);

signals:
    void testPlayRequested(const QString& rewardId);
    void emergencyStopTriggered();

private slots:
    // 演出管理タブ操作
    void onRewardSelected(int index);
    void onAddNewRewardClicked();
    void onTwitchSyncClicked();
    void onSaveSettingClicked();
    void onDeleteRewardClicked();
    void onTestPlayClicked();
    void onMediaTypeChanged(int index);
    void onPreviewPositionChanged(int newX, int newY);
    void onEmergencyStopClicked();

    // 統計タブ操作
    void onSessionSelected(int index);
    void onExportCsvClicked();

    // データ更新シグナル受け取り
    void refreshRewardList();
    void refreshAnalyticsTab();
    void updateQueueStatus(int current, int max);

private:
    void setupUi();
    QWidget* createHeaderBar();
    QWidget* createRewardManagerTab();
    QWidget* createAnalyticsTab();
    QWidget* createSettingsTab();

    ICoreContext* m_context;
    EffectManager* m_effectMgr;
    QueueManager* m_queueMgr;
    AnalyticsManager* m_analyticsMgr;

    // ヘッダーウィジェット
    QLabel* m_lblQueueStatus;
    QPushButton* m_btnEmergencyStop;

    // サブタブ
    QTabWidget* m_subTabWidget;

    // 報酬演出管理 UI
    QListWidget* m_lstRewards;
    QPushButton* m_btnNewReward;
    QPushButton* m_btnTwitchSync;

    QLineEdit* m_txtRewardId;
    QLineEdit* m_txtRewardName;
    QSpinBox* m_spnPoints;
    QSpinBox* m_spnCooldown;
    QComboBox* m_cmbPlaybackMode;
    QCheckBox* m_chkEnabled;

    QComboBox* m_cmbEffectIndex;
    QPushButton* m_btnAddEffect;
    QPushButton* m_btnDeleteEffect;
    QCheckBox* m_chkCustomHtml;
    QComboBox* m_cmbMediaType;
    QLineEdit* m_txtImagePath;
    QLineEdit* m_txtVideoPath;
    QLineEdit* m_txtAudioPath;
    QSpinBox* m_spnDuration;
    QSpinBox* m_spnSizePercent;
    QLineEdit* m_txtTextTemplate;
    QComboBox* m_cmbPositionPreset;
    QSpinBox* m_spnCenterX;
    QSpinBox* m_spnCenterY;

    VisualPreviewWidget* m_previewWidget;
    QPushButton* m_btnTestPlay;
    QPushButton* m_btnSaveSetting;
    QPushButton* m_btnDeleteReward;

    // アナリティクス UI
    QComboBox* m_cmbSessions;
    QPushButton* m_btnExportCsv;
    QLabel* m_lblTotalCount;
    QLabel* m_lblTotalPoints;
    QLabel* m_lblActiveUsers;
    QLabel* m_lblPopularReward;
    QTableWidget* m_tblRewardStats;
    QTableWidget* m_tblUserStats;

    // システム設定 UI
    QSpinBox* m_spnCanvasWidth;
    QSpinBox* m_spnCanvasHeight;
    QSpinBox* m_spnMaxQueue;
    QComboBox* m_cmbOverflowPolicy;
    QSpinBox* m_spnSessionGap;
    QLineEdit* m_txtLanIpGuide;
};

#endif // PLUGIN_MAIN_WIDGET_H
