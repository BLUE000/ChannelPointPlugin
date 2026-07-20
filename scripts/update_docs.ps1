param(
    [ValidateSet("get", "set", "")]
    [string]$WipAction = "",

    [string]$WipSourceFile,

    [string]$DecisionLogName
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 1. Process WIP_STATE.md (get or set)
if ($WipAction -eq "get") {
    $PhpWip = Join-Path $ScriptDir "manage_wip_state.php"
    php $PhpWip get
} elseif ($WipAction -eq "set" -or $WipSourceFile) {
    $PhpWip = Join-Path $ScriptDir "manage_wip_state.php"
    if ($WipSourceFile) {
        php $PhpWip set $WipSourceFile
    } else {
        php $PhpWip set
    }
}

# 2. Create new DecisionLog
if ($DecisionLogName) {
    $PhpDec = Join-Path $ScriptDir "create_decision_log.php"
    php $PhpDec $DecisionLogName
}
