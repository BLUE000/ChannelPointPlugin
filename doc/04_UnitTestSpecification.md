# 単体テスト仕様書 (Unit Test Specification) - ChannelPointPlugin

## 1. 概要
本ドキュメントは、詳細設計書 (`doc/03_DetailedDesignSpecification.md`) に対応する単体テスト（ユニットテスト）の仕様書です。
UI/UXのレイアウト・手動表示要素は除外され、各ロジッククラスのイベントトリガー受信後の自動処理・アルゴリズム・状態変化を Qt Test (`QTest`) を用いて自動化検証する仕様を規定します。

---

## 2. 自動テスト環境・方針
- **テストフレームワーク**: Qt Test (`QTest` / `ctest`)
- **自動実行コマンド**: `ctest --output-on-failure` またはビルド後のテストバイナリ実行
- **検証範囲**: UI描画自体を除外し、シグナル発火・イベントトリガー後のデータ計算、境界チェック、キュー操作、ファイル保存ロジックの完全自動化。

---

## 3. テスト項目一覧 (Test Cases)

### 3.1 `EffectManagerTest` (演出マスター管理クラスのテスト)
| テストID | テスト対象関数 / イベント | 入力パラメータ / トリガー | 期待される動作・戻り値 | 自動検証方法 |
| :--- | :--- | :--- | :--- | :--- |
| **UT-EFF-01** | `loadSettings()` / `saveSettings()` | 設定ファイル保存シグナル / ダミーデータ | JSONフォーマットで正しくシリアライズ・デシリアライズされること | 読み込み後の構造体比較 |
| **UT-EFF-02** | `getSettingForReward()` | 未登録の `rewardId` | デフォルト演出設定 (`effect_default`) が返却されること | `QCOMPARE` で演出ID検証 |
| **UT-EFF-03** | `updateSetting()` | 有効な `RewardEffectSetting` | 該当IDの設定が更新され、変更シグナルが発火すること | `QSIGNALSPY` で更新通知検出 |

### 3.2 `QueueManagerTest` (ID参照型Queue & 緊急停止のテスト)
| テストID | テスト対象関数 / イベント | 入力パラメータ / トリガー | 期待される動作・戻り値 | 自動検証方法 |
| :--- | :--- | :--- | :--- | :--- |
| **UT-QUE-01** | `pushQueue()` | `QueueItem` (1件) | キューカウントが1となり、タイマーが起動して順次処理が開始されること | `queueCountChanged` シグナルキャッチ |
| **UT-QUE-02** | `pushQueue()` (Overflow) | キュー上限(20件)を超過する 21 件目の追加 | `DropOldest` 設定に基づき最古のキューが破棄され、サイズ20が維持されること | `QCOMPARE(queue.size(), 20)` |
| **UT-QUE-03** | `emergencyStop()` (緊急停止トリガー) | キューに5件溜まっている状態で緊急停止シグナル発火 | **キューサイズが即座に 0 にクリアされ、OBSへ消去コマンド送出シグナルが即時発行されること** | シグナルアサート & `QCOMPARE(size, 0)` |

### 3.3 `VisualPreviewWidgetTest` (はみ出し境界チェックアルゴリズムのテスト)
| テストID | テスト対象関数 / イベント | 入力パラメータ / トリガー | 期待される動作・戻り値 | 自動検証方法 |
| :--- | :--- | :--- | :--- | :--- |
| **UT-BND-01** | `checkBoundary()` | $X=200, Y=150$ (1920x1080 枠内) | `isOutOfBounds == false`, `clampedX == 200, clampedY == 150` | 戻り値構造体のプロパティ比較 |
| **UT-BND-02** | `checkBoundary()` | $X=-100, Y=2000$ (完全な枠外) | `isOutOfBounds == true`, `clampedX` と `clampedY` が枠線内に正常にクランピングされること | 補正後の範囲比較 ($0 \le X \le 1920$) |

### 3.4 `AnalyticsManagerTest` (統計 & 自動セッション切替のテスト)
| テストID | テスト対象関数 / イベント | 入力パラメータ / トリガー | 期待される動作・戻り値 | 自動検証方法 |
| :--- | :--- | :--- | :--- | :--- |
| **UT-ANA-01** | `recordRedemption()` | `TwitchRewardRedemption` イベント | 該当ユーザーおよび報酬のカウント・ポイント合計が正確に加算されること | `QCOMPARE` で数値アサート |
| **UT-ANA-02** | `recordRedemption()` (Gap) | 前回から2時間以上離れたタイムスタンプのイベント | 既存セッションが非アクティブ化され、**新配信セッションが自動生成されること** | セッションID件数の変化検証 |

---

## 4. 自動テスト実行スクリプト・コード構造
Qt Test マクロ (`QTEST_MAIN`) を使用し、無人・自動実行が可能なテストドライバーを作成します。
