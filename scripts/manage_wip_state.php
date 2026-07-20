<?php
// scripts/manage_wip_state.php

$wipFile = dirname(__DIR__) . '/WIP_STATE.md';

// 1. アクションの取得
$action = isset($argv[1]) ? strtolower(trim($argv[1])) : '';
if (empty($action) || !in_array($action, ['get', 'set'])) {
    echo "アクションを選択してください (get / set): \n";
    $action = strtolower(trim(fgets(STDIN)));
}

if ($action === 'get') {
    // 表示処理
    if (!file_exists($wipFile)) {
        echo "WIP_STATE.md が存在しません。\n";
        exit(0);
    }
    $content = file_get_contents($wipFile);
    echo $content;
    exit(0);
} elseif ($action === 'set') {
    // 書き換え処理
    $newContent = '';

    // 引数でロード元ファイルが指定されている場合
    if (isset($argv[2]) && !empty($argv[2])) {
        $sourcePath = $argv[2];
        if (!file_exists($sourcePath)) {
            echo "Error: 入力ファイルが見つかりません: {$sourcePath}\n";
            exit(1);
        }
        $newContent = file_get_contents($sourcePath);
    } else {
        // 標準入力から複数行受け取る (WindowsでのCtrl+DやCtrl+Zのパースハングを防ぐため、:q または exit での終了をサポート)
        echo "新しい WIP_STATE.md の内容を入力してください。\n";
        echo "(終了するには、新しい行で ':q' または 'exit' と入力して Enter を押してください。Windowsは Ctrl+Z -> Enter でも終了できます):\n";
        echo "--------------------------------------------------\n";
        $lines = [];
        while (true) {
            $line = fgets(STDIN);
            if ($line === false) {
                break; // EOF検知
            }
            $trimmed = trim($line);
            if ($trimmed === ':q' || $trimmed === 'exit') {
                break; // 明示的な終了文字
            }
            $lines[] = $line;
        }
        $newContent = implode('', $lines);
    }

    if (empty(trim($newContent))) {
        echo "Error: 新しい内容が空です。処理を中止します。\n";
        exit(1);
    }

    // 古いPHPバージョン互換用の関数定義
    if (!function_exists('str_contains')) {
        function str_contains($haystack, $needle) {
            return $needle !== '' && strpos($haystack, $needle) !== false;
        }
    }

    // 必須セクション見出しのバリデーション (AI等による誤った上書き破壊を防ぐセーフティ)
    $hasRequiredSection = false;
    $requiredSections = ['## 最近の実装完了タスク', '## 現在の作業', '## 今後の機能拡張アイデア'];
    foreach ($requiredSections as $sec) {
        if (str_contains($newContent, $sec)) {
            $hasRequiredSection = true;
            break;
        }
    }

    if (!$hasRequiredSection) {
        echo "Error: WIP_STATE.md に必要な重要見出し (例: '## 最近の実装完了タスク' または '## 現在の作業') が見つかりません。書き込みを安全に中止しました。\n";
        exit(1);
    }

    // サイズの大幅な減少に対するバリデーション (コンテンツの意図しない激減・破壊を防ぐ)
    if (file_exists($wipFile)) {
        $oldSize = filesize($wipFile);
        $newSize = strlen($newContent);
        
        // 元のサイズが一定以上(100B以上)あり、新しいサイズが元の20%未満になっている場合は異常とみなす
        if ($oldSize > 100 && $newSize < ($oldSize * 0.2)) {
            echo "Error: 新しい内容のサイズ ({$newSize}B) が、元のサイズ ({$oldSize}B) の 20% 未満です。コンテンツの大部分が消失している可能性があるため、書き込みを安全に中止しました。\n";
            exit(1);
        }
    }

    // UTF-8エンコーディングのチェック (文字コードの破損防止)
    if (!mb_check_encoding($newContent, 'UTF-8')) {
        echo "Error: 新しい内容が有効な UTF-8 ではありません。書き込みを安全に中止しました。\n";
        exit(1);
    }

    // バックアップ作成
    if (file_exists($wipFile)) {
        $currentContent = file_get_contents($wipFile);
        $currentContent = str_replace(["\r\n", "\r"], "\n", $currentContent); // LF統一
        
        // Git管理外の scripts/backup フォルダ内に格納
        $backupDir = __DIR__ . '/backup';
        if (!is_dir($backupDir)) {
            mkdir($backupDir, 0777, true);
        }
        
        $backupFile = $backupDir . '/' . date('Y-m-d_H-i-s') . '_WIP_STATE.md';
        if (file_put_contents($backupFile, $currentContent) !== false) {
            echo "Backup Created: {$backupFile}\n";
        } else {
            echo "Error: バックアップの作成に失敗したため、上書き処理を中止します。\n";
            exit(1);
        }
    }

    // 新規コンテンツをLF統一して書き込み
    $newContent = str_replace(["\r\n", "\r"], "\n", $newContent);
    
    // 安全なアトミック書き込み (一時ファイルに書いてからリネームで上書き)
    $tmpFile = $wipFile . '.tmp';
    if (file_put_contents($tmpFile, $newContent) !== false) {
        if (rename($tmpFile, $wipFile)) {
            echo "Success: WIP_STATE.md を更新しました。(UTF-8 LF)\n";
        } else {
            echo "Error: 一時ファイルから WIP_STATE.md への置き換え（リネーム）に失敗しました。\n";
            if (file_exists($tmpFile)) {
                unlink($tmpFile);
            }
            exit(1);
        }
    } else {
        echo "Error: 一時ファイルへの書き込みに失敗しました。\n";
        exit(1);
    }
} else {
    echo "Error: 無効なアクションです。'get' または 'set' を指定してください。\n";
    exit(1);
}
