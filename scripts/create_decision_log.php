<?php
// scripts/create_decision_log.php

$targetDir = dirname(__DIR__) . '/DecisionLog';
if (!is_dir($targetDir)) {
    mkdir($targetDir, 0777, true);
}

// 1. ファイル名の取得
$fileName = isset($argv[1]) ? trim($argv[1]) : '';
if (empty($fileName)) {
    echo "DecisionLogのファイル名を入力してください (例: 2026-07-01_23-22-00_DecisionLog): \n";
    $fileName = trim(fgets(STDIN));
}

if (empty($fileName)) {
    echo "Error: ファイル名が入力されませんでした。\n";
    exit(1);
}

// 拡張子の補正
if (pathinfo($fileName, PATHINFO_EXTENSION) !== 'md') {
    $fileName .= '.md';
}

$filePath = $targetDir . '/' . $fileName;

// 重複チェック
if (file_exists($filePath)) {
    echo "Error: ファイルが既に存在します: {$filePath}\n";
    exit(1);
}

// 2. テンプレート作成
$currentTime = date('Y-m-d H:i:s');
$template = <<<EOT
# 意思決定ログ ({$currentTime})

## 1. 変更対象フェーズ
- **フェーズ:** 

## 2. 変更・追加ファイル
- 

## 3. 変更理由 (Why)
- 
EOT;

// 改行コードを確実に LF (\n) に変換
$template = str_replace(["\r\n", "\r"], "\n", $template);

// 3. 書き込み
if (file_put_contents($filePath, $template) !== false) {
    echo "Success: 意思決定ログを作成しました:\n";
    echo "File: {$filePath}\n";
} else {
    echo "Error: ファイルの書き込みに失敗しました。\n";
    exit(1);
}
