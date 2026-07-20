# ChannelPointPlugin

![Plugin Icon](pic/ChannelPoint.png)

Twitch 配信管理ツール（`TwitchChannelManagementTool`）向けのチャンネルポイント演出制御・使用率アナリティクス拡張プラグインです。

---

## 概要 (Overview)

本プラグインは、Twitch のチャンネルポイント（Channel Point Reward）が引き換えられた際に、OBSブラウザソースへの動的な演出表示（画像・音声・動画・複合アニメーション）を行うとともに、配信ごとのチャンネルポイント利用率やリスナー貢献度のアナリティクス機能を提供します。

---

## 主な機能 (Key Features)

### 1. 🎁 報酬演出管理 (Effect Management)
- **多機能メディアサポート**: 画像、音声（SE）、動画（MP4/WebM等）、画像+音声、動画+音声、カスタムHTML/CSS/JSの複合演出に対応。
- **視覚的プレビュー & はみ出しチェック (Boundary Check)**: 2Dプレビュー画面上で位置・サイズを視覚的に調整可能。画面外へのはみ出しを自動判定・補正。
- **演出の柔軟な割り当て**: Twitch報酬IDごとに演出ルールを登録・マッピング可能。

### 2. ⚡ ID参照型Queue & 緊急停止 (Queue & Emergency Stop)
- **軽量Queue制御**: イベントデータと演出ID参照のみでキューを構成し、高速かつ順次再生（Queue方式）をサポート。
- **緊急停止機能 (Emergency Stop)**: クールタイム未設定や大量連打時、ボタンワンクリックで演出キュー全クリアおよびOBS画面の即時消去を実行。

### 3. 📊 使用率アナリティクス (Analytics & Statistics)
- **配信セッション自動判定**: 無操作時間ギャップから配信の「開始」「終了」を自動切替・分離管理。
- **集計ダッシュボード**: 報酬別利用回数・消費ポイント、リスナー別貢献ランキング表示およびCSVデータエクスポート。

### 4. 🌐 2PC配信・同一LAN接続対応 (2PC & Remote OBS Support)
- **非ハードコーディング構成**: IPアドレスやポート番号の固定依存を排除。
- **外部OBS接続**: 同一LAN内の別PCで稼働するOBSブラウザソース（`http://<ホストPCのIP>:<PORT>/`）からの接続・演出受信を完全にサポート。

---

## 開発・ビルド要件 (Requirements)

- **C++規格**: C++20
- **ビルドツール**: CMake 3.20 以上
- **フレームワーク**: Qt 6 (Core, Widgets, Network, Test)
- **ホストアプリ**: [TwitchChannelManagementTool](https://github.com/BLUE000/TwitchChannnelManagementTool.git)

---

## ドキュメント一覧 (Documentation)

開発プロセス（Vモデル）に基づく各設計およびテストドキュメントは `doc/` ディレクトリに管理されています。

- 📋 [要件定義書 (Requirements Specification)](doc/01_RequirementsSpecification.md)
- 📐 [基本設計書 (Basic Design Specification)](doc/02_BasicDesignSpecification.md)
- 🔬 [詳細設計書 (Detailed Design Specification)](doc/03_DetailedDesignSpecification.md)
- 🧪 [単体テスト仕様書 (Unit Test Specification)](doc/04_UnitTestSpecification.md)
- 🔗 [結合テスト仕様書 (Integration Test Specification)](doc/05_IntegrationTestSpecification.md)
- 🌐 [システムテスト仕様書 (System Test Specification)](doc/06_SystemTestSpecification.md)

---

## ライセンス (License)

[MIT License](LICENSE)
