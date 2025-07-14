#pragma once
#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

// DECLARE_LOG_CATEGORY_EXTERN 매크로를 사용하여 LogAura 로그 카테고리를 선언
// LogAura: 로그 카테고리 이름
// Log: 기본 로그 레벨.
// All: 컴파일 시 로그 레벨(모든 로그를 포함).
DECLARE_LOG_CATEGORY_EXTERN(LogAura, Log, All);